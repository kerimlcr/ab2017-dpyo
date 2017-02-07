
/* Copyright (C) 2011-2016  Lucio Carreras
 *
 * This file is part of sayonara player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */




#ifndef TREE_H
#define TREE_H


#include <QList>
#include <algorithm>

#include "Helper/Logger/Logger.h"

template<typename T>
/**
 * @brief The Tree class
 * @ingroup Helper
 */
class Tree {

	public:
		Tree* parent=nullptr;

		QList<Tree*> children;
		T data;

		/**
		 * @brief Tree constructor
		 * @param data_ set the root element
		 */
		Tree(const T& data_){
			data = data_;
			parent = nullptr;
			children.clear();
		}

		virtual ~Tree(){
			for(Tree* child : children){
				delete child; child = nullptr;
			}

			children.clear();
			data = T();
		}

		/**
		 * copy the entire tree. Has to be deleted afterwards
		 */
		Tree* copy(){
			Tree* node = new Tree(this->data);

			for(Tree* child : children){
				node->children << child->copy();
			}

			return node;
		}


		/**
		 * @brief adds a child to the given node
		 * @param node the parent node
		 * @return pointer to inserted node
		 */
		Tree* add_child(Tree* node){

			node->parent = this;
			this->children << node;

			this->sort(false);

			return node;
		}


		/**
		 * @brief remove a node from the current node
		 * @param deleted_node node to remove
		 * @return pointer to deleted_node
		 */
		Tree* remove_child(Tree* deleted_node){

			deleted_node->parent = nullptr;

			for(int i=0; i < children.size(); i++){

				Tree* node = children[i];

				if(node == deleted_node){
					deleted_node = children.takeAt(i);
					i--;
				}
			}

			return deleted_node;
		}


		/**
		 * @brief move current node to a new parent
		 * @param new_parent new parent of node
		 */
		void move(Tree* new_parent){

			parent->remove_child(data);
			new_parent->add_child(this);
		}

		/**
		 * @brief sort children of all nodes in ascending way according to their data
		 * @param recursive if set to true, do it for all subnodes, too
		 */
		void sort(bool recursive){
			int i;

			if(children.isEmpty()){
				return;
			}

			auto lambda = [](Tree* a, Tree* b){
				return (a->data < b->data);
			};

			std::sort(children.begin(), children.end(), lambda);


			i=0;
			for(Tree* child : children){
				if(recursive){
					child->sort(recursive);
				}
				i++;
			}
		}


		void print(int lvl) const {

			for(Tree* child : children){

				QString str;
				for(int i=0; i<lvl; i++){
					str += "  ";
				}

				str += "|_";

				child->print(lvl+1);
			}
		}

};


#endif // TREE_H
