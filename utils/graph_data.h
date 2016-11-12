/*
 * author:      allen lee(Junhao Li) allen_lee922@foxmail.com
 * address:     WNLO, Huazhong University of Science & Technology
 * Time:        2016.11
 *
 * Copyright 2015 Key Laboratory of Data Storage System, Ministry of Education WNLO HUST
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 *  This namespace dx_lib includes somes utils for our project.
 *  Class graph_data is a graph data abstract class
 *
 *
 */
#ifndef _GRAPH_DATA_H
#define _GRAPH_DATA_H_
#include <bits/stdc++.h>
//#include <boost/thread/thread.hpp>
//#include <boost/thread/mutex.hpp>
//#include <boost/thread/condition.hpp>
#include "log_wrapper.h"
#include "dispatcher.h"
#include "config.h"
namespace dx_lib{
    class graph_data{
        public:
            explicit graph_data(std::string graphfile_name)
                :m_graphdata_name(graphfile_name){            
    
            }
            void init(){
                format::config conf(m_graphdata_name+".conf");
                stringstream ss;

                ss << conf["type"];            //graph type
                ss >> type;
                ss.clear();

                ss << conf["vertices"];  //graph vertices numbers
                ss >> n_vertices;
                ss.clear();

                ss << conf["edges"];
                ss >> n_edges;        //graph edges numbers

            }
            //get graph type
            int get_type(){
                return type;
            }
            //get numbers of the graph vertices
            int get_vertices_count(){
                return n_vertices;
            }

            //get numbers of the graph edges
            int get_edges_count(){
                return n_edges;
            }

            ~graph_data(){
                
            }
            //friend inline dispatcher & operator<<(dispatcher out, graph_data &gd){
                
            //}
        private:
            const std::string m_graphdata_name;
            //std::string m_graphconfig_name;
            //format::config conf;
            int type;
            int n_vertices;
            int n_edges;
    };    
}
#endif
