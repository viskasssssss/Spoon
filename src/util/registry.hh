#pragma once

#include <utility>
#include <unordered_map>
#include <vector>
#include <functional>
#include <string>
#include <algorithm>

#include "log.hh"


namespace spoon {
    template <typename... Args>
    class function_registry {
    public:
        using function_type = std::function<void(Args...)>;
        using id_type = size_t;

        id_type register_function(function_type callback) {
            id_type id = next_id++;
            functions.emplace(id, std::move(callback));
            return id;
        }

        void remove_function(id_type id) {
            if (functions.erase(id) > 0) {
                free_ids.push_back(id);
            }
        }

        void execute_function(size_t id, Args... args) {
            auto it = functions.find(id);
            if (it != functions.end()) {
                it->second(std::forward<Args>(args)...); 
                auto md = std::find(marked_for_deletion.begin(), marked_for_deletion.end(), id);
                if (md != marked_for_deletion.end()) {
                    remove_function(id);
                }
            } else {
                SP_CORE_ERROR(
                    "Function ID not found: {0}",
                    id
                );
            }
        }

        void mark_for_deletion(size_t id) {
            marked_for_deletion.push_back(id);
        }

        void clear() {
            functions.clear();
            free_ids.clear();
            next_id = 0;
        }

        bool empty() {
            return functions.empty();
        }

        size_t size() {
            return functions.size();
        }

        auto begin() { return functions.begin(); }
        auto end() { return functions.end(); }

        auto begin() const { return functions.begin(); }
        auto end() const { return functions.end(); }

    private:
        size_t next_id = 1;
        std::unordered_map<size_t, std::function<void(Args...)>> functions;
        std::vector<id_type> free_ids;
        std::vector<id_type> marked_for_deletion;
    };
}
