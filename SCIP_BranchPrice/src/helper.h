//
//  helper.h
//  NDSR_ColGen
//
//  Created by Naga V Gudapati on 1/3/20.
//  Copyright © 2020 Naga V Gudapati. All rights reserved.
//

#ifndef helper_h
#define helper_h
#include <algorithm>
#include <set>
#include <stack>
#include <vector>

template <typename T> void print_vector(const std::vector<T>& vec)
{
        for (const auto& elem : vec)
        {
                std::cout << elem << " ";
        }
        std::cout << std::endl;
}

template <typename T> void print_set(const std::set<T>& uset)
{
        for (const auto& elem : uset)
        {
                std::cout << elem << " ";
        }
        std::cout << std::endl;
}

template <typename T> void print_path(const std::vector<T>& vec)
{

        if (vec.size() > 0)
        {
                for (auto it = vec.begin(); it != vec.end() - 1; ++it)
                {
                        // if the current index is needed:
                        std::cout << *it << " -> ";
                }
                std::cout << *(vec.end() - 1) << "\n";
        }
        else
        {
                std::cout << "BAD path! size is " << vec.size() << "\n";
        }
}

/*a comparator for sorting a set*/
struct compareWeights
{
        template <typename T> bool operator()(const T& l, const T& r) const
        {
                return l.second < r.second || (l.second == r.second && l.first < r.first);
        }
};

template <typename T> class vector_set : public std::vector<T>
{
    public:
        using iterator   = typename std::vector<T>::iterator;
        using value_type = typename std::vector<T>::value_type;

        std::pair<iterator, bool> insert(const value_type& val)
        {
                auto it = std::find(this->begin(), this->end(), val);
                if (it == this->end())
                        it = std::vector<T>::insert(this->end(), val);

                return std::pair<iterator, bool>(it, true);
        }

        bool has_elem(const value_type& val)
        {
                if (std::count(this->begin(), this->end(), val))
                        return true;
                else
                        return false;
        }

        void erase_elem(const value_type& val)
        {
                this->erase(std::remove(this->begin(), this->end(), val), this->end());
        }

        void print_vec_set()
        {
                for (const auto& elem : *this)
                {
                        std::cout << elem << " ";
                }
                std::cout << std::endl;
        }
};

#endif /* helper_h */
