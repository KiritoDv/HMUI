#include "FocusManager.h"
#include <cmath>
#include <algorithm>
#include <limits>
#include <iostream>

std::shared_ptr<FocusManager> FocusManager::instance = nullptr;

void FocusManager::registerNode(std::shared_ptr<FocusNode> node) {
    nodes.push_back(node);
    // Auto-focus the first element if nothing is focused
    if (!currentFocus) {
        currentFocus = node;
        if (currentFocus->onFocus) currentFocus->onFocus();
    }
}

void FocusManager::unregisterNode(std::shared_ptr<FocusNode> node) {
    // 1. Remove from main list
    nodes.erase(std::remove(nodes.begin(), nodes.end(), node), nodes.end());

    // 2. If we are removing the CURRENT focus, we need to reset
    if (currentFocus == node) {
        currentFocus = nullptr; // Temporarily null

        // Walk backwards through history to find a survivor
        while (!focusHistory.empty()) {
            auto weakNode = focusHistory.back();
            auto strongNode = weakNode.lock();
            focusHistory.pop_back(); // Remove from top (we are either processing it or it's dead)

            // If this node is still alive AND it's not the one we are currently deleting
            if (strongNode && strongNode != node) {
                // We found a previous node that is still valid!
                // Note: We call internal logic directly to avoid re-pushing to history recursively
                currentFocus = strongNode;
                if (currentFocus->onFocus) currentFocus->onFocus();

                // Re-add to history as the active tip
                focusHistory.push_back(currentFocus);
                break;
            }
        }
    }
    // 3. Fallback: If history was empty or all nodes dead (e.g. whole app reset)
    // Try to focus the first available node in the registry
    if (!currentFocus && !nodes.empty()) {
        setFocus(nodes.front());
    }
}

void FocusManager::clear() {
    nodes.clear();
    focusHistory.clear();
    currentFocus = nullptr;
}

bool FocusManager::isFocused(const std::shared_ptr<FocusNode>& node) {
    return currentFocus == node;
}

void FocusManager::submit() {
    if (currentFocus && currentFocus->onSubmit) {
        currentFocus->onSubmit();
    }
}

Rect FocusManager::getRect(const std::weak_ptr<InternalDrawable>& ptr) {
    if (auto widget = ptr.lock()) {
        // We need ABSOLUTE coordinates.
        // Assuming InternalDrawable::getBounds returns relative/local bounds
        // You might need to implement a 'getAbsoluteBounds' in InternalDrawable 
        // OR rely on the fact that GestureDetector captures 'absoluteRect' in onDraw.
        return widget->getBounds(); 
    }
    return Rect(0,0,0,0);
}

// Simple center-point calculation
struct Point { float x, y; };
Point center(const Rect& r) { return { r.x + r.width/2, r.y + r.height/2 }; }

void FocusManager::moveFocus(FocusDirection dir) {
    if (!currentFocus) return;

    Rect currentRect = getRect(currentFocus->widget);
    Point c1 = center(currentRect);

    std::shared_ptr<FocusNode> bestCandidate = nullptr;
    float bestScore = std::numeric_limits<float>::max();

    for (auto& node : nodes) {
        if (node == currentFocus) continue;
        
        // Ensure widget is still valid
        if (node->widget.expired()) continue;

        Rect targetRect = getRect(node->widget);
        Point c2 = center(targetRect);

        bool isValidDir = false;
        float distSq = std::pow(c2.x - c1.x, 2) + std::pow(c2.y - c1.y, 2);

        // Filter by direction
        // We prioritize items roughly in the cone of the direction
        switch (dir) {
            case FocusDirection::Up:
                isValidDir = c2.y < c1.y; 
                break;
            case FocusDirection::Down:
                isValidDir = c2.y > c1.y;
                break;
            case FocusDirection::Left:
                isValidDir = c2.x < c1.x;
                break;
            case FocusDirection::Right:
                isValidDir = c2.x > c1.x;
                break;
        }

        if (isValidDir) {
            // Scoring: Distance is main factor.
            // You can improve this by penalizing misalignment on the secondary axis.
            if (distSq < bestScore) {
                bestScore = distSq;
                bestCandidate = node;
            }
        }
    }

    if (bestCandidate) {
        setFocus(bestCandidate);
        std::cout << "Focused: " << currentFocus->id << "\n";
    }
}

void FocusManager::setFocus(std::shared_ptr<FocusNode> node) {
    if (currentFocus && currentFocus->onBlur) {
        currentFocus->onBlur();
    }

    currentFocus = node;

    if (currentFocus) {
        if (currentFocus->onFocus) currentFocus->onFocus();
        focusHistory.push_back(currentFocus);
    }
}