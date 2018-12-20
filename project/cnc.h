

//cnc.h

class gcitem
{
};

typedef boost::shared_ptr< gcitem > sp_gcitem_t;

typedef std::vector< sp_gcitem_t > vect_sp_gcitem_t;
typedef std::vector< sp_gcitem_t >::iterator vect_sp_gcitem_it_t;

//straight rapid move
class gc0 : public gcitem
{
	bg_point a;
};
typedef boost::shared_ptr< gc0 > sp_gc0;

//straight feed move
class gc1 : public gcitem
{
	bg_point a;
	GEO_NUM f;
};
typedef boost::shared_ptr< gc1 > sp_gc1;

class gc_process
{
	vect_sp_gcitem_t process;
public:
	vect_sp_gcitem_it_t begin( ) { return process.begin( ); }
	vect_sp_gcitem_it_t end( ) { return process.end( ); }
	void push_back( sp_gcitem_t sp_item ) { process.push_back( sp_item ); }
};

