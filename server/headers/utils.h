#pragma once

#include <unordered_map>
#include <string_view>
#include <vector>
#include <tuple>
#include <stdexcept>
#include <utility>

class Utils{
    public:
        //myTypes
        using queryArgs = std::unordered_map<std::string, std::string>;


        static inline queryArgs ConstructQuery(std::vector<std::string_view> && query_name, std::tuple<>){
            if(!query_name.empty()){
                throw std::runtime_error("Parameter names provided n arguments.");
            }
            return {};
        }

        //template function
        template<typename... Args>
        static queryArgs ConstructQuery(std::vector<std::string_view> && query_name, std::tuple<Args...> && query_args){
            return ConstructQueryImpl(std::move(query_name), 
            std::forward<std::tuple<Args...>>(query_args),
        std::index_sequence_for<Args...>{});
        }

    private:
        //Impl
        template<typename... Args, std::size_t... Is>
        static queryArgs ConstructQueryImpl(std::vector<std::string_view> && query_name,
        std::tuple<Args...> && t,
        std::index_sequence<Is...>){
            if(query_name.size() != sizeof...(Args)){
                throw std::runtime_error("Number of parameter's name does not match arguments.");
            }

            queryArgs result;

            (result.emplace(std::string(query_name[Is]), 
                ConvertString(std::get<Is>(t))), ...);
            return result;
        }


        //private function
        template<typename T>
        static std::string ConvertString(T&& value) {
            if constexpr (std::is_same_v<std::decay_t<T>, std::string_view>) {
                return "\"" + std::string(value) + "\"";
            } else if constexpr (std::is_same_v<std::decay_t<T>, std::string>) {
                return "\"" + value + "\"";
            } else if constexpr (std::is_arithmetic_v<std::decay_t<T>>) {
                return std::to_string(value);
            } else {
                static_assert(std::is_arithmetic_v<std::decay_t<T>>, 
                            "Unsupported type for ConvertString");
                return "";
            }
        }
};