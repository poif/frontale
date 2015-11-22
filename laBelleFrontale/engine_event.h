
#ifndef H__engine_event__
#define H__engine_event__

#include <string>
#include <map>

/**
* generic class 
*/
class engine_event{
public:
	int type;
	std::map<std::string, std::string> s_data;
	std::map<std::string, int> i_data;

	bool operator==(const engine_event& e){
		return (type == e.type) && 
			   (s_data == e.s_data) && 
			   (i_data == e.i_data);
	}

	template<class Archive>
	void serialize(Archive& ar, const unsigned int){
		ar & type;
		ar & s_data;
		ar & i_data;
	}

	enum{
		LOOK,
		LOOKREC,
		SHOW,
		SHOWREC,
		EXIST,
		ANSWER,
		PUSH,
		PULL,

	};

};

#endif 