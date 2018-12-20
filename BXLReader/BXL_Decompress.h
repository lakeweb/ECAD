
#if !defined(BXL_DECOMPRESS_MAY_04_2018)
#define BXL_DECOMPRESS_MAY_04_2018
#pragma once


#ifdef _DEBUG
extern std::ofstream bxl_decompress_os;
#define OS_BXL_DECOMPRESS( x ) { bxl_decompress_os << x; }
#else
#define OS_BXL_DECOMPRESS( x ) __noop;
#endif

#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/smart_ptr/enable_shared_from_this.hpp>

namespace BXLReader
{
	// ........................................................
	// ........................................................
	class Node : public boost::enable_shared_from_this< Node >
	{
	public:
		using NodePtr = boost::shared_ptr< Node >;
		using WNodePtr = boost::weak_ptr< Node >;
		int      level;
		WNodePtr parent;
		NodePtr  left;
		NodePtr  right;
		int      symbol;
		int      weight;
		// ........................................................
		Node( )
			:level( 0 )
			,parent( )
			,left( )
			,right( )
			,symbol( -1 )
			,weight( 0 )
		{
			OS_BXL_DECOMPRESS("Empty Node created" << std::endl; );
		}
		// ........................................................
		Node(NodePtr inparent, int insymbol)
			:level( 0 )
			,parent(inparent)
			,left( )
			,right( )
			,symbol( -1 )
			,weight( 0 )
		{
			if( inparent )
			{
				if( auto p= parent.lock())
					level= p->level + 1;
			}
			else
				level= 0;

			if( level > 7 )
			{
				symbol= insymbol;
			}
		}
		// ........................................................
		static NodePtr MakeNode( ) { return boost::make_shared< Node >( ); }
		static NodePtr MakeNode(Node& parent, int insym) { return boost::make_shared< Node >(parent.shared_from_this(), insym);	}
		void incrementWeight() { weight++; }
		bool is_leaf() { return(level > 7); }
		// ........................................................
		NodePtr sibling(NodePtr node)
		{
			if (node != right)
				return right;
			else
				return left;
		}
		// ........................................................
		bool need_swapping()
		{
			auto p = parent.lock();
			if (p && p->parent.lock() && /* root node */ weight > p->weight)
				return true;
			//else
			return false;
		}
		// ........................................................
		NodePtr add_child( int insymbol )
		{
			if( level < 7 )
			{
				if( right )
				{
					auto ret= right->add_child( insymbol );
					if( ret )
						return ret;
				}
				if( left )
				{
					auto ret= left->add_child( insymbol );
					if( ret )
						return ret;
				}
				if (!right)
				{
					right = MakeNode(*this, -1);
					return right;
				}
				if (!left)
				{
					left = MakeNode(*this, -1);
					return left;
				}
				return NodePtr( );
			}
			else
			{
				if (!right)
				{
					right = MakeNode(*this, insymbol);
					return right;
				}
				else if (!left)
				{
					left = MakeNode(*this, insymbol);
					return left;
				}

				else
					return NodePtr( ); // Leaves are filled
			}
		}
	};//Node

	  // ........................................................
	  // ........................................................
	class NodeTree
	{
	public:
		using NodePtr = Node::NodePtr;
		NodePtr root;
		// ........................................................
		NodeTree( )
			:root( Node::MakeNode( ) )
		{
			int leaf_count= 0;
			NodePtr node = root;
			for( ; node; )
			{
				node= root->add_child( leaf_count );
				if( node && node->is_leaf( ) )
					++leaf_count;
			}
			OS_BXL_DECOMPRESS( "Final leaf_count is: " << leaf_count << std::endl );
		}
		// ........................................................
		NodePtr getRoot( ) { return root; }
		// ........................................................
		void swap (NodePtr n1, NodePtr n2, NodePtr n3)
		{
			if( n3 )
				n3->parent= n1;
			if( n1->right == n2 )
			{
				n1->right= n3;
				return;
			}
			if( n1->left == n2 )
				n1->left= n3;
		}
		// ........................................................
		void update_tree(NodePtr current)
		{
			// System.out.println("Updating tree...");
			if( current && current->need_swapping( ) )
			{
				auto parent= current->parent.lock( );
				auto grand_parent = parent->parent.lock( );
				auto parent_sibling = grand_parent->sibling( parent );
				swap( grand_parent, parent,  current );
				swap( grand_parent, parent_sibling, parent );
				swap( parent, current, parent_sibling );
				parent->weight= parent->right->weight + parent->left->weight;
				grand_parent->weight = current->weight + parent->weight;
				update_tree( parent );
				update_tree( grand_parent );
				update_tree( current );
			}
		}
	};//NodeTree

	  // ........................................................
	  // ........................................................
	class Buffer
	{
	public:
		using NodePtr = Node::NodePtr;
		NodeTree tree;
		char size_buffer[4];
		std::streamsize source_size;
		std::streamsize index;
		char cur_char;
		int bit;
		//if reading stream
		std::istream* ptr_is;
		std::string res;
		// ........................................................
		Buffer(std::istream& in)
			:size_buffer( )
			, source_size( 0 )
			, bit(0)
			, index(4)
			, cur_char(0)
			, tree()
		{
			ptr_is = &in;
			in.read(size_buffer, 4);
			std::streamsize out_file_length = get_uncompressed_size( );
			res.resize((size_t)out_file_length);
		}
		// ........................................................
		std::string decode_stream( )
		{
			auto it = res.begin();
			for ( ; it != res.end( ); )
			{
				NodePtr node = tree.root;
				for (; !node->is_leaf(); )
				{
					// find leaf node
					if (read_next_stream() != 0)
						node = node->left;
					else
						node = node->right;
				}

				*it++ = (char)node->symbol; // more efficient string building, thanks wlbaker
				node->incrementWeight();
				tree.update_tree(node);
			}
			return res;
		}
		// ........................................................
		int read_next_stream()
		{
			int result = 0;
			if (bit < 0)
			{
				bit = 7;
				ptr_is->read( &cur_char, 1 );
				result = cur_char & (1 << bit);
			}
			else
				result = cur_char & (1 << bit);

			--bit;
			return result;
		}
		// ........................................................
		std::streamoff get_uncompressed_size()
		{
			/* Uncompressed size =
			B0b7 * 1<<0 + B0b6 * 1<<1 + ... + B0b0 * 1<<7 +
			B1b7 * 1<<0 + B1b6 * 1<<1 + ... + B2b0 * 1<<7 +
			B2b7 * 1<<0 + B2b6 * 1<<1 + ... + B3b0 * 1<<7 +
			B3b7 * 1<<0 + B3b6 * 1<<1 + ... + B4b0 * 1<<7
			*/
			size_t size = 0;
			size_t mask = 0;
			for (size_t i = 7; i != -1; i--)
			{
				if ((size_buffer[0] & 1 << i) != 0)
					size |= (1 << mask);
				mask++;
			}
			for (int i = 7; i >= 0; i--)
			{
				if ((size_buffer[1] & 1 << i) != 0)
					size |= (1 << mask);
				mask++;
			}
			for (int i = 7; i >= 0; i--)
			{
				if ((size_buffer[2] & 1 << i) != 0)
					size |= (1 << mask);
				mask++;
			}
			for (int i = 7; i >= 0; i--)
			{
				if ((size_buffer[3] & 1 << i) != 0)
					size |= (1 << mask);
				mask++;
			}
			return size;
		}
	};// Buffer

}//namespace BXL

#endif //BXL_DECOMPRESS_MAY_04_2018
