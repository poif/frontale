
#ifndef H__engine_event__
#define H__engine_event__

#include <string>
#include <map>



/**
* generic class to manage events between engines
*/
class engine_event{
public:
	int type;
	std::map<std::string, std::string> s_data;
	std::map<std::string, int> i_data;

	bool operator==(const engine_event& e){
		return (type == e.type) && 
			   (s_data == e.s_data) && 
			   (i_data == e.i_data) && 
			   (v_data == e.v_data);
	}

	template<class Archive>
	void serialize(Archive& ar, const unsigned int){
		ar & type;
		ar & s_data;
		ar & i_data;
		ar & v_data;
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

/*
list of all graphics_events : 
type		key		value

///////////////////////////////////////////////////////
list of all network_events : 
type		key		value
MSG_NICK	NICK	player's nick
			PASS	password to join the game
GAME_PROP	MAP		map name
			NAME	name of the server
			NB_PLAY	max number of players
WELCOME		none
REFUSED		REASON	explanation (nick already existing, wrong password, game full ...)
NEW_PLAYER	NAME	name of the player
RUN_MULTI	none
SEND_GAME_PROP	none
GAME_HAS_STARTED none	informs the network engine that the game has started
CHOSEN_MAP	NAME	name of the chosen map
DEL_PLAYER	NAME	name of the player to remove
LOAD		none	used for the game round to do the graphics engine load the round
ADD_NODE	ID		id of the mesh to add
			POS		position to put the node at
			ROT		orientation to put the node at
MOVE_NODE	ID
			POS
			ROT
REM_NODE	ID		id of the node to delete
REMOVE_MACHINE ID	id of the network machine to remove
*/
