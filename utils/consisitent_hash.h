#ifndef _CONSISTENT_HASH_
#define _CONSISTENT_HASH_
#include <bits/stdc++.h>
#include <climits>
#include <cstdint>
#include "graph_data.h"
//能快速查找一个key对应的机器node
//选取一个好的key hash函数
#define MIN_VERTIECS_PER_NODE 10000
namespace dx_lib{
    /*class hash_func{
    public:
         
    }
    
    //template<typename key_type>
    class vtx_hash:public hash_func{
    public:
        vtx_hash(){}
        uint32_t operator()(uint32_t vertex_id){
            return vertex_id;      
        }
    };*/

    class compute_node{

    private:
        //ma存放计算节点在ring环中的节点到机器节点的映射。
        std::map<uint32_t, uint32_t> node_to_ring;
        //uint32_t compute_node_count;
        //noeds 存放计算节点列表， 以uint32_t表示一个计算节点。
        std::vector<uint32_t> m_nodes;
        dx_lib::graph_data m_graph_data;
        const uint32_t min_vertices_per_node;



    public:
        compute_node(std::vector<uint32_t> &machine_nodes, 
                        dx_lib::graph_data &gd)
                        :m_nodes(machine_nodes),
                            m_graph_data(gd),
                            min_vertices_per_node(MIN_VERTICES_PER_NODE){

            
            if(!machine_nodes.empty()){
            
                uint32_t each = m_graph_data.get_vertices_count() / m_nodes.size();
            
                each = each>min_vertices_per_node? each:min_vertices_per_node;
            
                //映射
                for(int32_t i=1; i <= m_nodes.size(); i++){
                    //i*each is bigger than UINT_MAX is OK
                    node_to_ring[i*each] = m_nodes[i-1];
                }
            }
        }



        uint32_t vertex_to_ring_value(uint32_t vertex_id){
        //this is the function that map the vertex to the ring
            return vertex_id;
        }

        /*uin32_t node_to_ring_value(uint32_t node_id){
            
        }*/



        uint32_t split(uint32_t node_id){
        //the compute node node_id is too busy, so it should be splited
            
            //add the compute node
            //TODO

        }

        uint32_t operator()(uint32_t vertex_id){
        //return node_id
            auto iter = node_to_ring.lower_bound(vertex_to_ring_value(vertex_id));
            if(iter!=node_to_ring.end()){
                return iter->second; //return the nearest compute node
            }
            else{
                return node_to_ring.begin()->second;
            }
            
        }
    };

    //key_func and node func is class function, must implement func(), and return uint32
    template<typename vertex_func, typename node_func>
    class consistent_hash{
        consistent_hash(){
            
        }
    };
}

#endif
