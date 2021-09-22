/**
@file
@brief file with 2q cache data structure.
*/

#include <unordered_map>
#include <list>
#include <cassert>

namespace coolnamespace
{

    /**
     * Data structure that implements the 2Q caching algorithm.
     * @tparam T the type of pages stored in cache
     * @tparam KeyT type of page address
    **/
    template <typename T, typename KeyT = int>
    class Cache2Q
    {
        private:
            /// queue codes
            enum {Q_INPUT = 0, Q_OUTPUT = 1, Q_MAIN = 2};

            /// struct for list node, contains page data and the key of this page
            struct list_node {KeyT key_; T data_;};

            /// struct for hash node, contains iterator to required list node and queue code (Q_INPUT, Q_OUTPUT or Q_MAIN)
            struct hash_node {int q_type_; typename std::list<list_node>::iterator iter;};

            /// listsize_[Q_CODE] - max size of input/output/main queue
            size_t listsize_[3];

            /// listsize_[Q_CODE] - input/output/main queue
            std::list<list_node> pagelist_[3];

            /// hashtable for ~O(1) search
            std::unordered_map<KeyT, hash_node> hash_;

            /**
             * move page from one queue to front of other (maybe same) queue
             * @param node hash node of page we want to move
             * @param q_type code of queue we want to move to
            **/
            void list_move_to_other_front (hash_node node, int q_type)
            {
                pagelist_[q_type].splice (pagelist_[q_type].begin(), pagelist_[node.q_type_], node.iter);
                hash_.find(pagelist_[q_type].begin()->key_)->second.q_type_ = q_type;
                if (pagelist_[q_type].size() > listsize_[q_type])
                {
                    hash_.erase (hash_.find(pagelist_[q_type].back().key_));
                    pagelist_[q_type].pop_back();
                }
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
                assert (in_size + out_size + main_size > 0);

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
                    if (node->second.q_type_ == Q_INPUT)
                        list_move_to_other_front (node->second, Q_INPUT);
                    else if (node->second.q_type_ == Q_OUTPUT)
                        list_move_to_other_front (node->second, Q_MAIN);
                    else if (node->second.q_type_ == Q_MAIN)
                        list_move_to_other_front (node->second, Q_MAIN);

                    return &((node->second.iter)->data_);
                }
                else
                {
                    pagelist_[Q_INPUT].push_front (list_node{page, loadpage (page)});
                    hash_[page] = {Q_INPUT, pagelist_[Q_INPUT].begin()};
                    if (pagelist_[Q_INPUT].size() > listsize_[Q_INPUT])
                        list_move_to_other_front (hash_.find (pagelist_[Q_INPUT].rbegin()->key_)->second, Q_OUTPUT);
                    
                    return &(pagelist_[Q_INPUT].front().data_);
                }
            }
    };
}