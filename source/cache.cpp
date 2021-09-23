/**
@file
@brief file with 2q cache data structure.
*/

#include <unordered_map>
#include <list>
#include <cassert>
#include <iostream>
#include <iterator>

namespace ppmipt
{
    /// queue codes
    enum {Q_INPUT = 0, Q_OUTPUT = 1, Q_MAIN = 2};

    /**
     * Data structure that implements the 2Q caching algorithm.
     * @tparam T the type of pages stored in cache
     * @tparam KeyT type of page address
    **/
    template <typename T, typename KeyT = int>
    class Cache2Q
    {
        private:
            /// struct for list node, contains page data and the key of this page
            struct list_node {KeyT key_; T data_;};

            /// struct for hash node, contains iterator to required list node and queue code (Q_INPUT, Q_OUTPUT or Q_MAIN)
            struct hash_node {int q_code_; typename std::list<list_node>::iterator iter;};

            /// listsize_[Q_CODE] - max size of input/output/main queue
            size_t listsize_[3] = {0, 0, 0};

            /// listsize_[Q_CODE] - input/output/main queue
            std::list<list_node> pagelist_[3];

            /// hashtable for ~O(1) search
            std::unordered_map<KeyT, hash_node> hash_;

            /**
             * move page from one queue to front of other (maybe same) queue
             * @param node hash node of page we want to move
             * @param q_code code of queue we want to move to
            **/
            void list_move_to_other_front (hash_node node, int q_code)
            {
                pagelist_[q_code].splice (pagelist_[q_code].begin(), pagelist_[node.q_code_], node.iter);
                hash_[pagelist_[q_code].begin()->key_] = {q_code, pagelist_[q_code].begin()};
                if (pagelist_[q_code].size() > listsize_[q_code])
                {
                    hash_.erase (pagelist_[q_code].back().key_);
                    pagelist_[q_code].pop_back();
                }
            }

            /**
             * load and push new page to input queue
             * @param page address of page we want to get
             * @param loadpage page loading funtion
            **/
            void push_new_page (KeyT page, T (*loadpage)(KeyT))
            {
                pagelist_[Q_INPUT].push_front (list_node{page, loadpage (page)});
                hash_[page] = {Q_INPUT, pagelist_[Q_INPUT].begin()};
                if (pagelist_[Q_INPUT].size() > listsize_[Q_INPUT])
                    list_move_to_other_front (hash_.find (pagelist_[Q_INPUT].back().key_)->second, Q_OUTPUT);
            }

        public:
            /**
             * Constructor...
             * @param in_size input queue size
             * @param out_size output queue size
             * @param main_size main queue size
            **/
            Cache2Q (size_t in_size, size_t out_size, size_t main_size)
            {
                listsize_[Q_INPUT] = in_size;
                listsize_[Q_OUTPUT] = out_size;
                listsize_[Q_MAIN] = main_size;
            }

            // Destructor, but why...
            ~Cache2Q () {};

            /**
             * get page from cache or slowly load it
             * @param page address of page we want to get
             * @param loadpage page loading funtion
             * @param counter external counter for cache hits
            **/
            const T* const get_page (KeyT page, T (*loadpage)(KeyT), unsigned long* counter = nullptr)
            {
                auto node = hash_.find(page);
                if (node != hash_.end())
                {
                    if (counter != nullptr)
                        *counter += 1;
                    if (node->second.q_code_ == Q_INPUT)
                        list_move_to_other_front (node->second, Q_INPUT);
                    else if (node->second.q_code_ == Q_OUTPUT && listsize_[Q_MAIN] > 0)
                        list_move_to_other_front (node->second, Q_MAIN);
                    else if (node->second.q_code_ == Q_MAIN)
                        list_move_to_other_front (node->second, Q_MAIN);

                    return &((node->second.iter)->data_);
                }
                else
                {
                    push_new_page (page, loadpage);
                    return &(pagelist_[Q_INPUT].front().data_);
                }
            }

            /**
             * resize one of cache queue
             * @param q_code code of queue 
             * @param new_size new size of queue
            **/
            void resize (int q_code, size_t new_size)
            {
                assert (new_size >= 0);

                if (listsize_[q_code] <= new_size)
                {
                    listsize_[q_code] = new_size;
                    return;
                }

                auto iter = pagelist_[q_code].begin();
                std::advance (iter, new_size);
                for (auto i = iter; i != pagelist_[q_code].end(); i++)
                    hash_.erase (i->key_);

                pagelist_->erase (iter, pagelist_[q_code].end());
            };

            /**
             * get size of max queue size
             * @param q_code code of required queue
             * @return max size
            **/
            size_t get_size (int q_code) const
            {
                return listsize_[q_code];
            };
    };
}