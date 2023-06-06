#pragma once
#include <memory>

template<typename T>
struct flattener;

template<typename T>
struct FlatteningLifoQueue {
    struct Node {
        Node *next {nullptr};
        std::unique_ptr<Node> prev{nullptr};
        size_t observers{0};
        T val;

        static void exclude(Node* node) {
            auto& prev = node->prev;
            auto next = node->next;
            if(prev)
                prev->next = next;
            if(next)
                next->prev = std::move(prev);
        }

        static void flatten_chain(Node* node) {
            if(node->observers > 0)
                return;

            while(node->prev && node->prev->observers == 0) {
                node = node->prev.get();
            }

            flattener<T> f;
            while(node->next && node->next->observers == 0) {
                auto next = node->next;
                f(next->val, node->val);
                exclude(node);
                node = next;
            }
        }
    };
    std::unique_ptr<Node> top{nullptr};

    struct Observer {
        Node* node;
        T& get() {
            return node->val;
        }
        Observer(Node* n) : node(n) {
            ++(node->observers);
        }
        Observer(Observer&& that) : node(that.node) {
            that.node = nullptr;
        }
        ~Observer() {
            if(!node)
                return;
            --(node->observers);
            Node::flatten_chain(node);
        }
    };

    void push(T v) {
        auto node = std::make_unique<Node>();

        if(top) {
            top->next = node.get();
        }

        node->prev = std::move(top);
        node->val = std::move(v);

        top = std::move(node);

        Node::flatten_chain(top.get());
    }
    Observer observe() {
        return {top.get()};
    }
};

