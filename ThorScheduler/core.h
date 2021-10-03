#pragma once
#include "function.h"

#define HEAD					5
#define UNIT					2
#define STATUS_NUMS				4
#define INFO_P					6
#define MAX_VAL					1001

#define FIRST_FLOOR				1
#define FIRST_ELEVATOR			1
#define MIN_NUM_PASSENGER		1
#define MIN_TIME_COST			1

#define DIRECTION_STOP			9
#define DIRECTION_STAY			0
#define DIRECTION_CLOCK			1
#define DIRECTION_COUNTER		-1

#define ADDED_CODE				0x00
#define ENTER_CODE				0x01
#define EOF_CODE				0x02

#define STAY_MODE				0x00
#define CARRY_MODE				0x01
#define MOVE_UP_MODE			0x01
#define MOVE_DOWN_MODE			0x02

#define NONE_CODE				0x00
#define OPEN_CODE				0x01
#define MOVE_CODE				0x02
#define FREE_CODE				0x07

#define ACCEPT_CODE				0x00
#define TERMINATE_CODE			0x09

#define TRANSPORT_MODE			0x77
#define READY_TO_BOARD			0x55
#define READY_TO_MOVE			0x44

#define ALIGHT_CODE				0x00
#define BOARD_CODE				0x01
#define APPROACH_CODE			0x02
#define REVERSE_CODE			0x09

#define READY_PROGRAMME			0x08
#define READY_HEADER			0x09

typedef map<int32_t, int16_t> SCHEDULE_MAP;
struct map_data {
	unsigned char flow;
	int32_t floor;
	int32_t lastStop;
	unsigned char status;
	int8_t direction;
	SCHEDULE_MAP boarded;
	SCHEDULE_MAP alighting;
	int16_t sum;
	int8_t attempts;
	vector<int32_t> schedule;
	vector<int32_t> scheduleTemp;
};
typedef vector<vector<int32_t>> PASSENGER_MAT;
typedef unordered_map<int32_t, map_data> MAP;

namespace ThorTower {
	class core
	{
		public:
			core();
			~core();

			int8_t getInput();

			void printValues();
			void printElevator();
			void printFloor();
			void printPassenger();
			vector<string> isValidPassenger(vector<string> data);
			int8_t isValidHeader(const vector<string>& data);

			int8_t updateByElevator();
			void assignAgain();

			int8_t hasQueue(const int32_t& index);
			int8_t checkPassengers(const int32_t& index);
			int8_t moveUp(const int32_t& index);
			int8_t moveDown(const int32_t& index);
			void allArrived(const int32_t& index);
			void allAlighted(const int32_t& index);
			int8_t carryPassengers(const int32_t& index);
			void boardPassengers(const int32_t& index);

			void cleanElevator(const int32_t& index);

			bool addPassenger(const vector<string>& data);
			void assignElevator(const int16_t& passengerIndex);

			int32_t findEleToPpl(int32_t eleIdx, int32_t startIdx);
			int32_t findWaitingPpl(int32_t startIdx);
			int32_t findStartFloorToPpl(int32_t floorIdx, int32_t startIdx);
			int32_t findEndFloorToPpl(int32_t floorIdx, int32_t startIdx);
			int32_t findStartEndFloorToPpl(int32_t floorIdx, int32_t startIdx);

			Functions::function func;
		private:
			int8_t* ready;

			int16_t* passengerNum;

			string* inputStr;
			string tempSchedule;

			/* Headers */
			int32_t* elevatorNums;
			int32_t* floorNums;
			int32_t* timeTakenToMove;
			int32_t* timeTakenOnOff;
			int16_t* passenserLimit;
			int16_t* passenserNums;

			/* Data */
			PASSENGER_MAT passengerSet;
			MAP elevatorMap;

			/* Status */
			vector<string> strStatus;

			/* Indices */
			enum eFlow
			{
				checkBoarded = 0,
				approach,
				up,
				down,
				checkAlighted,
				clean
			};
			enum eStatus
			{
				stop = 0,
				upward,
				downward,
				opened
			};
			enum pStatus
			{
				added = 0,
				assigned,
				boarding,
				alighted
			};
			enum header
			{
				floors = 0,
				elevators,
				secToMove,
				secOnOff,
				passengers
			};
			enum go
			{
				num = 0,
				from,
				to,
				elevator,
				status,
				direction
			};
	};
}

