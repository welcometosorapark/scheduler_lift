#include "core.h"

namespace ThorTower {
	core::core() :
		ready(new (nothrow) int8_t),
		inputStr(new (nothrow) string),
		elevatorNums((int32_t*)calloc(1, sizeof(int32_t))),
		floorNums((int32_t*)calloc(1, sizeof(int32_t))),
		timeTakenToMove((int32_t*)calloc(1, sizeof(int32_t))),
		passengerNum((int16_t*)calloc(1, sizeof(int16_t))),
		passenserLimit((int16_t*)calloc(1, sizeof(int16_t))),
		timeTakenOnOff((int32_t*)calloc(1, sizeof(int32_t)))
	{
		strStatus.resize(STATUS_NUMS);
		strStatus[0] = "S";
		strStatus[1] = "U";
		strStatus[2] = "D";
		strStatus[3] = "O";
		if (!inputStr) inputStr = new (nothrow) string;
		if (!ready) ready = new (nothrow) int8_t;
		if (ready)
		{
			*ready = READY_PROGRAMME;
		}
	}
	core::~core()
	{
		// Delete pointers allocated with calloc()
		free(passengerNum);
		free(elevatorNums);
		free(floorNums);
		free(timeTakenToMove);
		free(passenserLimit);
		free(timeTakenOnOff);
		// Delete pointer allocated with new
		delete inputStr;
		// clear data and reduce the capacity of the array
		strStatus.clear();
		strStatus.shrink_to_fit();
		elevatorMap.clear();
		passengerSet.clear();
		passengerSet.shrink_to_fit();
		printf("core.cpp closed.\n");
	}

	int8_t core::getInput()
	{
		try
		{
			if (!core::inputStr) new (nothrow) string;
			if (core::inputStr)
			{
				// Receive input string
				getline(cin, *core::inputStr);
				if (cin.eof())
				{
					if (*ready == READY_HEADER)
					{
						while (true) // Update and print data until the system ends
						{
							return updateByElevator();
						}
					}
				}
				else  if (*core::inputStr == "") // Print the latest update
				{
					if (*ready == READY_HEADER)
					{
						printValues();
						//updateByElevator();
					}
				}
				else
				{
					vector<string> data = func.spaceOut((*core::inputStr).c_str());
					if (*ready == READY_HEADER) // Only allow to add passengers when the header data has been arrived.
					{
						if (addPassenger(data))
						{
							return updateByElevator();
						}
					}
					else
					{
						if (*ready == READY_PROGRAMME) // Get header if the programe is ready to work.
						{
							*ready = isValidHeader(data);
						}
					}
					vector<string>().swap(data);
				}
			}
			return ACCEPT_CODE;
		}
		catch (const exception & e)
		{
			func.msgError(e, "core_getInput");
		}
		return false;
	}

	void core::printValues()
	{
		try
		{
			printElevator();
			printFloor();
			printPassenger();
		}
		catch (const exception & e)
		{
			func.msgError(e, "core_printValues");
		}
	}

	void core::printElevator()
	{
		try
		{
			string* strE = new (nothrow) string;
			int16_t* startIdx = new (nothrow) int16_t;
			int16_t* matched = new (nothrow) int16_t;
			if (!strE) strE = new (nothrow) string;
			if (!startIdx) startIdx = new (nothrow) int16_t;
			if (!matched) matched = new (nothrow) int16_t;
			if (strE && startIdx && matched)
			{
				for (int i = 1; i <= elevatorMap.size(); i++)
				{
					*strE = ("E" + to_string(i));
					*strE += (" F" + to_string(elevatorMap[i].floor));
					*strE += (" " + strStatus[elevatorMap[i].status]);
					*startIdx = 0;
					while ((*matched = findEleToPpl(i, *startIdx)) != (end(passengerSet) - begin(passengerSet)))
					{
						//printf("[ Queue ] %d => num: %d, status: %d, size: %d\n", i, \
							passengerSet[matched][core::go::num], passengerSet[matched][core::go::status], passengerSet.size());
						*startIdx = (*matched + 1);
						if (passengerSet[*matched][core::go::status] == core::pStatus::boarding || \
							passengerSet[*matched][core::go::status] == core::pStatus::alighted)
						{
							*strE += (" P" + to_string(passengerSet[*matched][core::go::num]));
						}
					}
					*strE += "\n";
					printf("%s", (*strE).c_str());
				}
				delete strE;
				delete startIdx;
				delete matched;
			}
		}
		catch (const exception & e)
		{
			func.msgError(e, "core_printElevator");
		}
	}

	void core::printFloor()
	{
		try
		{
			string* fStr = new (nothrow) string;
			int* fStartIdx = new (nothrow) int;
			int32_t* fMatched = new (nothrow) int32_t;
			if (!fStr) fStr = new (nothrow) string;
			if (!fStartIdx) fStartIdx = new (nothrow) int;
			if (!fMatched) fMatched = new (nothrow) int;
			if (fStr && fStartIdx && fMatched)
			{
				for (int i = FIRST_FLOOR; i <= *floorNums; i++)
				{
					*fStartIdx = 0;
					*fStr = ("F" + to_string(i));

					while (((*fMatched) = findStartFloorToPpl(i, *fStartIdx)) != (end(passengerSet) - begin(passengerSet)))
					{
						*fStartIdx = (*fMatched + 1);
						if (passengerSet[*fMatched][core::go::status] == core::pStatus::added || \
							passengerSet[*fMatched][core::go::status] == core::pStatus::assigned)
						{
							*fStr += (" P" + to_string(passengerSet[*fMatched][core::go::num]));
						}
					}

					*fStr += "\n";
					tempSchedule += *fStr;
					printf("%s", (*fStr).c_str());
				}
				delete fStr;
				delete fStartIdx;
				delete fMatched;
			}
		}
		catch (const exception & e)
		{
			func.msgError(e, "core_printFloor");
		}
	}

	void core::printPassenger()
	{
		try
		{
			string* pStr = new (nothrow) string;
			if (!pStr) pStr = new (nothrow) string;
			if (pStr)
			{
				for (int i = 0; i < passengerSet.size(); i++)
				{
					//printf("¡Ú¡Ú printPassenger (%d) ==> %d: %d\n", passengerSet.size(),\
						passengerSet[i][core::go::num], passengerSet[i][core::go::status]);
					*pStr = "P" + to_string(passengerSet[i][core::go::num]);
					switch (passengerSet[i][core::go::status])
					{
					case core::pStatus::added:
						*pStr += (" F" + to_string(passengerSet[i][core::go::from]));
						if (passengerSet[i][core::go::elevator] >= FIRST_ELEVATOR && \
							elevatorMap[passengerSet[i][core::go::elevator]].floor == passengerSet[i][core::go::from])
						{
							*pStr += (" E" + to_string(passengerSet[i][core::go::elevator]));
						}
						break;
					case core::pStatus::assigned:
						*pStr += (" F" + to_string(passengerSet[i][core::go::from]));
						*pStr += (" E" + to_string(passengerSet[i][core::go::elevator]));
						break;
					case core::pStatus::boarding:
						*pStr += (" E" + to_string(passengerSet[i][core::go::elevator]));
						break;
					case core::pStatus::alighted:
						*pStr += (" E" + to_string(passengerSet[i][core::go::elevator]));
						*pStr += (" F" + to_string(passengerSet[i][core::go::to]));
						break;
					}

					*pStr += "\n";
					printf("%s", (*pStr).c_str());
				}
				delete pStr;
			}
		}
		catch (const exception & e)
		{
			func.msgError(e, "core_printPassenger");
		}
	}

	int8_t core::updateByElevator()
	{
		try
		{
			if (passengerSet.size() > 0)
			{
				for (int i = 1; i <= elevatorMap.size(); i++)
				{
					if (elevatorMap[i].flow == core::eFlow::checkBoarded ||\
						elevatorMap[i].flow == core::eFlow::clean)
					{
						if (elevatorMap[i].flow == core::eFlow::clean)
						{
							cleanElevator(i);
						}

						assignAgain();
						switch (hasQueue(i))
						{
						case STAY_MODE:
							break;
						case CARRY_MODE:
							switch (carryPassengers(i))
							{
							case READY_TO_BOARD:
								boardPassengers(i);
								elevatorMap[i].attempts = 0;
								break;
							case READY_TO_MOVE:
								break;
							}
							break;
						}
					}
					else if (elevatorMap[i].flow == core::eFlow::up)
					{
						switch (moveUp(i))
						{
						case ALIGHT_CODE:
							allArrived(i);
							break;
						case BOARD_CODE:
							break;
						case TRANSPORT_MODE:
							break;
						}
					}
					else if (elevatorMap[i].flow == core::eFlow::down)
					{
						switch (moveDown(i))
						{
						case ALIGHT_CODE:
							allArrived(i);
							break;
						case BOARD_CODE:
							break;
						case TRANSPORT_MODE:
							break;
						}
					}
					else if (elevatorMap[i].flow == core::eFlow::checkAlighted)
					{
						allAlighted(i);
					}
				}
				printValues();
				if (passengerSet.size() == 0)
				{
					return TERMINATE_CODE;
				}
			}
			return ACCEPT_CODE;
		}
		catch (const exception & e)
		{
			func.msgError(e, "core_updateByElevator");
		}
	}

	int8_t core::hasQueue(const int32_t& index)
	{
		try
		{
			int16_t* startIdx = new (nothrow) int16_t;
			int16_t* queueMatched = new (nothrow) int16_t;
			if (!startIdx) startIdx = new (nothrow) int16_t;
			if (!queueMatched) queueMatched = new (nothrow) int16_t;
			if (queueMatched && startIdx)
			{
				bool* newlyAssigned = new (nothrow) bool(false);
				if (!newlyAssigned) newlyAssigned = new (nothrow) bool(false);
				if (newlyAssigned)
				{
					*startIdx = 0;
					map<int32_t, vector<int32_t>> fromArray;
					map<int32_t, bool> toArray;
					while ((*queueMatched = findEleToPpl(index, *startIdx)) != (end(passengerSet) - begin(passengerSet)))
					{
						*startIdx = (*queueMatched + 1);
						fromArray[passengerSet[*queueMatched][core::go::from]].push_back(passengerSet[*queueMatched][core::go::to]);
					}
					elevatorMap[index].scheduleTemp.clear();
					elevatorMap[index].scheduleTemp.shrink_to_fit();
					// Compare the values with its arrival and the next arrival floors
					// and insert between them if the value(s) is in case as given below.
					int fromCount = elevatorMap[index].floor; 
					for (auto k : fromArray)
					{
						elevatorMap[index].scheduleTemp.push_back(k.first);
						for (int j = 0; j < fromArray[k.first].size(); j++)
						{
							if (fromArray[k.first][j] > fromCount && fromArray[k.first][j] < prev(fromArray.end())->first)
							{
								// Push the value in ascending order.
								elevatorMap[index].scheduleTemp.push_back(fromArray[k.first][j]);
							}
							else
							{
								toArray[fromArray[k.first][j]] = true;
							}
						}
						fromCount = k.first;
					} 
					// Insert the remaining values in descending order.
					for (auto rit = toArray.rbegin(); rit != toArray.rend(); ++rit)
					{
						elevatorMap[index].scheduleTemp.push_back(rit->first);
					} 
					// Find out the index of the current floor at which the elevator is located
					int32_t copyIdx = -1;
					for (int n = 0; n < elevatorMap[index].scheduleTemp.size(); n++)
					{
						if (elevatorMap[index].schedule.size() > 0 && elevatorMap[index].scheduleTemp[n] == elevatorMap[index].schedule[0])
						{
							copyIdx = n;
							break;
						}
					} 
					fromArray.clear();
					toArray.clear();
					elevatorMap[index].schedule.clear();
					elevatorMap[index].schedule.shrink_to_fit();
					// Set up the state of the elevator depending the updated data
					if (elevatorMap[index].scheduleTemp.size() > 0)
					{
						if (copyIdx > -1)
						{
							elevatorMap[index].schedule.insert(elevatorMap[index].schedule.end(), elevatorMap[index].scheduleTemp.begin() + copyIdx, elevatorMap[index].scheduleTemp.end());
						}
						else
						{
							elevatorMap[index].schedule = elevatorMap[index].scheduleTemp;
						}
					}

					int8_t nextStep = STAY_MODE;
					if (elevatorMap[index].scheduleTemp.size() > 0)
					{
						elevatorMap[index].status = core::eStatus::opened;
						nextStep = CARRY_MODE;
					}
					else
					{
						elevatorMap[index].status = core::eStatus::stop;
						nextStep = STAY_MODE;
					}
					delete newlyAssigned;
					return nextStep;
				}
				delete queueMatched;
				delete startIdx;
			}
		}
		catch (const exception & e)
		{
			func.msgError(e, "core_hasQueue");
		}
	}

	int8_t core::carryPassengers(const int32_t& index)
	{
		try
		{
			int16_t* count = new (nothrow) int16_t;
			int32_t* matched = new (nothrow) int32_t;
			int32_t* startIdx = new (nothrow) int32_t;
			bool* newlyAssigned = new (nothrow) bool(false);
			if (!count) new (nothrow) int16_t;
			if (!matched) new (nothrow) int32_t;
			if (!startIdx) new (nothrow) int32_t(0);
			if (!newlyAssigned) new (nothrow) bool(false);
			if (newlyAssigned && startIdx && matched && count)
			{
				if (elevatorMap[index].schedule[0] != elevatorMap[index].floor)
				{
					// Determine the next direction and update the value as selected.
					switch (func.getDirection(elevatorMap[index].floor, elevatorMap[index].schedule[0]))
					{
					case DIRECTION_CLOCK:
						elevatorMap[index].status = core::eStatus::downward;
						elevatorMap[index].flow = core::eFlow::down;
						break;
					case DIRECTION_COUNTER:
						elevatorMap[index].status = core::eStatus::upward;
						elevatorMap[index].flow = core::eFlow::up;
						break;
					}
					return READY_TO_MOVE;
				}
				if (elevatorMap[index].schedule.size() > 0)
				{
					*count = 0;
					*startIdx = 0;
					while ((*matched = findStartFloorToPpl(elevatorMap[index].schedule[0], *startIdx)) != (end(passengerSet) - begin(passengerSet)))
					{
						*startIdx = (*matched + 1);
						auto fromIdx = find(elevatorMap[index].schedule.begin(), elevatorMap[index].schedule.end(), passengerSet[*matched][core::go::from]);
						auto toIdx = find(elevatorMap[index].schedule.begin() + (fromIdx - elevatorMap[index].schedule.begin()), elevatorMap[index].schedule.end(), passengerSet[*matched][core::go::to]);
						if (toIdx != elevatorMap[index].schedule.end())
						{
							(*count) += 1;
							// Update the passenger's status from added to assigned 
							// if the passenger belongs to the elevator and the current status is 'added'.
							if (passengerSet[*matched][core::go::status] == core::pStatus::added && passengerSet[*matched][core::go::elevator] == index)
							{
								passengerSet[*matched][core::go::status] = core::pStatus::assigned;
								*newlyAssigned = true;
								break;
							}
						}
					}
				}
				elevatorMap[index].attempts++;
				// Move on to the next step.
				if (*newlyAssigned)
				{
					if ((func.abs(elevatorMap[index].sum - elevatorMap[index].alighting[elevatorMap[index].floor]) >= *passenserLimit && \
						elevatorMap[index].lastStop == elevatorMap[index].floor))
					{
						return READY_TO_BOARD;
					}
				}
				if (!(*newlyAssigned) && (*count == elevatorMap[index].sum || elevatorMap[index].attempts > 1))
				{
					return READY_TO_BOARD;
				}
				if (!(*newlyAssigned))
				{
					switch (func.getDirection(elevatorMap[index].floor, elevatorMap[index].schedule[0]))
					{
					case DIRECTION_CLOCK:
						elevatorMap[index].flow = core::eFlow::down;
						break;
					case DIRECTION_COUNTER:
						elevatorMap[index].flow = core::eFlow::up;
						break;
					}
					return READY_TO_MOVE;
				}
				delete newlyAssigned;
				delete startIdx;
				delete matched;
				delete count;
			}
		}
		catch (const exception & e)
		{
			func.msgError(e, "core_carryPassengers");
		}
	}

	void core::boardPassengers(const int32_t& index)
	{
		try
		{
			int16_t* boarded = new (nothrow) int16_t;
			int16_t* alighting = new (nothrow) int16_t;
			int16_t* matched = new (nothrow) int16_t;
			int16_t* startIdx = new (nothrow) int16_t;
			int16_t* count = new (nothrow) int16_t;
			bool* newlyAssigned = new (nothrow) bool(false);
			if (!boarded) new (nothrow) int16_t;
			if (!alighting) new (nothrow) int16_t;
			if (!matched) new (nothrow) int16_t;
			if (!startIdx) new (nothrow) int16_t;
			if (!count) new (nothrow) int16_t;
			if (!newlyAssigned) new (nothrow) bool(false);
			if (newlyAssigned && count && startIdx && matched && alighting && boarded)
			{
				*count = 0;
				*startIdx = 0;
				*boarded = 0;
				*alighting = 0;
				while ((*matched = findEleToPpl(index, *startIdx)) != (end(passengerSet) - begin(passengerSet)))
				{
					(*count) += 1;
					*startIdx = (*matched + 1);
					if (passengerSet[*matched][core::go::from] == elevatorMap[index].floor)
					{
						// Count how many passengers are about to alight or board either.
						if (passengerSet[*matched][core::go::status] == core::pStatus::alighted)
						{
							(*alighting) += 1;
						}
						if (passengerSet[*matched][core::go::status] == core::pStatus::boarding)
						{
							(*boarded) += 1;
						}
						if (passengerSet[*matched][core::go::status] == core::pStatus::assigned)
						{
							passengerSet[*matched][core::go::status] = core::pStatus::boarding;
							*newlyAssigned = true;
							break;
						}
					}
				}
				if (!(*newlyAssigned) && *alighting > 0)
				{
					allArrived(index);
				}
				else if ((*newlyAssigned && elevatorMap[index].sum > 0 && elevatorMap[index].sum == *count) || \
					(!(*newlyAssigned) && *boarded > 0 && (*boarded == elevatorMap[index].boarded[elevatorMap[index].floor] || *boarded == elevatorMap[index].sum)))
				{
					if (elevatorMap[index].schedule.size() > 1 && elevatorMap[index].floor == elevatorMap[index].schedule[0])
					{
						switch (func.getDirection(elevatorMap[index].floor, elevatorMap[index].schedule[1]))
						{
						case DIRECTION_CLOCK:
							elevatorMap[index].status = core::eStatus::downward;
							elevatorMap[index].flow = core::eFlow::down;
							// Erase the current floor just before leaving the floor.
							elevatorMap[index].schedule.erase(elevatorMap[index].schedule.begin());
							elevatorMap[index].schedule.shrink_to_fit();
							break;
						case DIRECTION_COUNTER:
							elevatorMap[index].status = core::eStatus::upward;
							elevatorMap[index].flow = core::eFlow::up;
							// Erase the current floor just before leaving the floor.
							elevatorMap[index].schedule.erase(elevatorMap[index].schedule.begin());
							elevatorMap[index].schedule.shrink_to_fit();
							break;
						}
					}
					else if (elevatorMap[index].schedule.size() > 1 && elevatorMap[index].floor != elevatorMap[index].schedule[0])
					{
						switch (func.getDirection(elevatorMap[index].floor, elevatorMap[index].schedule[0]))
						{
						case DIRECTION_CLOCK:
							elevatorMap[index].status = core::eStatus::downward;
							elevatorMap[index].flow = core::eFlow::down;
							// Erase the current floor just before leaving the floor.
							elevatorMap[index].schedule.erase(elevatorMap[index].schedule.begin());
							elevatorMap[index].schedule.shrink_to_fit();
							break;
						case DIRECTION_COUNTER:
							elevatorMap[index].status = core::eStatus::upward;
							elevatorMap[index].flow = core::eFlow::up;
							// Erase the current floor just before leaving the floor.
							elevatorMap[index].schedule.erase(elevatorMap[index].schedule.begin());
							elevatorMap[index].schedule.shrink_to_fit();
							break;
						}
					}
				}
				delete newlyAssigned;
				delete startIdx;
				delete matched;
				delete count;
			}
		}
		catch (const exception & e)
		{
			func.msgError(e, "core_boardPassengers");
		}
	}

	int8_t core::checkPassengers(const int32_t& index)
	{
		try
		{
			if (elevatorMap[index].floor != elevatorMap[index].schedule[0])
			{
				return TRANSPORT_MODE;
			}
			else
			{
				int16_t* board = new (nothrow) int16_t;
				int16_t* alight = new (nothrow) int16_t;
				if (!board) new (nothrow) int16_t;
				if (!alight) new (nothrow) int16_t;
				if (board && alight)
				{
					*board = 0;
					*alight = 0;
					for (int i = 0; i < passengerSet.size(); i++)
					{
						if (passengerSet[i][core::go::elevator] == index)
						{
							// Count how many passengers are about to alight or board either.
							if (passengerSet[i][core::go::to] == elevatorMap[index].floor && \
								passengerSet[i][core::go::status] == core::pStatus::boarding)
							{
								(*alight) += 1;
							}
							if (passengerSet[i][core::go::from] == elevatorMap[index].floor)
							{
								if (elevatorMap[index].status == core::eStatus::downward && \
									passengerSet[i][core::go::to] < elevatorMap[index].floor && \
									passengerSet[i][core::go::status] == core::pStatus::assigned)
								{
									(*board) += 1;
								}
								else if (elevatorMap[index].status == core::eStatus::upward && \
									passengerSet[i][core::go::to] > elevatorMap[index].floor && \
									passengerSet[i][core::go::status] == core::pStatus::assigned)
								{
									(*board) += 1;
								}
							}
						}
					}
					if (*alight > 0)
					{
						return ALIGHT_CODE;
					}
					if (*board > 0)
					{

						elevatorMap[index].flow = core::eFlow::checkBoarded;
						return BOARD_CODE;
					}
					delete board;
					delete alight;
				}
			}
		}
		catch (const exception & e)
		{
			func.msgError(e, "core_boardPassengers");
		}
	}

	int8_t core::moveUp(const int32_t& index)
	{
		try
		{
			if (elevatorMap[index].status != core::eStatus::upward)
			{
				elevatorMap[index].status = core::eStatus::upward;
			}
			else
			{
				elevatorMap[index].floor++;
			}
			return checkPassengers(index);
		}
		catch (const exception & e)
		{
			func.msgError(e, "core_moveUp");
		}
	}

	int8_t core::moveDown(const int32_t& index)
	{
		try
		{
			if (elevatorMap[index].status != core::eStatus::downward)
			{
				elevatorMap[index].status = core::eStatus::downward;
			}
			else
			{
				elevatorMap[index].floor--;
			}
			return checkPassengers(index);
		}
		catch (const exception & e)
		{
			func.msgError(e, "core_moveUp");
		}
	}

	void core::assignAgain()
	{
		try
		{
			int16_t* startIdx = new (nothrow) int16_t;
			int16_t* matched = new (nothrow) int16_t;
			if (!startIdx) new (nothrow) int16_t;
			if (!matched) new (nothrow) int16_t;
			if (startIdx && matched)
			{
				*startIdx = 0;
				while (((*matched) = findWaitingPpl(*startIdx)) != (end(passengerSet) - begin(passengerSet)))
				{
					*startIdx = ((*matched) + 1);
					// Only allow to be assigned in case that the passenger is never assigned.
					if (passengerSet[*matched][core::go::elevator] < FIRST_ELEVATOR)
					{
						assignElevator(*matched);
					}
				}
				delete startIdx;
				delete matched;
			}
		}
		catch (const exception & e)
		{
			func.msgError(e, "core_assignAgain");
		}
	}

	void core::allArrived(const int32_t& index)
	{
		try
		{
			int16_t* count = new (nothrow) int16_t;
			int16_t* startIdx = new (nothrow) int16_t;
			int16_t* matched = new (nothrow) int16_t;
			if (!count) new (nothrow) int16_t;
			if (!startIdx) new (nothrow) int16_t;
			if (!matched) new (nothrow) int16_t;
			if (startIdx && matched && count)
			{
				*count = 0;
				*startIdx = 0;
				while ((*matched = findEndFloorToPpl(elevatorMap[index].floor, *startIdx)) != (end(passengerSet) - begin(passengerSet)))
				{
					*startIdx = (*matched + 1);
					// Only allow to be assigned in case that the passenger is on the current floor in the target elevator6 3 5 1 .
					if (passengerSet[*matched][core::go::elevator] == index && \
						passengerSet[*matched][core::go::to] == elevatorMap[index].floor)
					{
						(*count) += 1;
						passengerSet[*matched][core::go::status] = core::pStatus::alighted;
					}
				}
				elevatorMap[index].status = core::eStatus::opened;
				elevatorMap[index].flow = core::eFlow::checkAlighted;
				delete startIdx;
				delete matched;
				delete count;
			}
		}
		catch (const exception & e)
		{
			func.msgError(e, "core_allArrived");
		}
	}

	void core::allAlighted(const int32_t& index)
	{
		try
		{
			elevatorMap[index].flow = core::eFlow::clean;
		}
		catch (const exception & e)
		{
			func.msgError(e, "core_allAlighted");
		}
	}

	void core::cleanElevator(const int32_t& index)
	{
		try
		{
			// Initialise the values delete correspoding rows of the passenger array.
			elevatorMap[index].flow = core::eFlow::checkBoarded;
			elevatorMap[index].status = core::eStatus::stop;
			elevatorMap[index].alighting.erase(elevatorMap[index].floor);
			elevatorMap[index].lastStop = elevatorMap[index].floor;
			elevatorMap[index].schedule.erase(elevatorMap[index].schedule.begin());
			elevatorMap[index].schedule.shrink_to_fit();
			for (int i = 0; i < passengerSet.size(); i++)
			{
				if (passengerSet[i][core::go::elevator] == index &&\
					passengerSet[i][core::go::to] == elevatorMap[index].floor)
				{
					elevatorMap[index].sum--;
					auto b = elevatorMap[index].boarded.find(passengerSet[i][core::go::to]);
					if (b != elevatorMap[index].boarded.end())
					{
						if (elevatorMap[index].boarded[passengerSet[i][core::go::from]] > 1)
						{
							elevatorMap[index].boarded[passengerSet[i][core::go::from]]--;
						}
						else
						{
							elevatorMap[index].boarded.erase(passengerSet[i][core::go::from]);
						}
					}
					passengerSet.erase(passengerSet.begin() + i);
					passengerSet.shrink_to_fit();
					i--;
				}
			}
		}
		catch (const exception & e)
		{
			func.msgError(e, "core_cleanElevator");
		}
	}

	bool core::addPassenger(const vector<string>& data)
	{
		try
		{
			// Split the given string by space
			vector<string> acceptedData = isValidPassenger(data);
			if (acceptedData.size() > 0)
			{
				int16_t* index = new (nothrow) int16_t;
				if (!index) new int16_t;
				if (index)
				{
					for (int i = 0; i < acceptedData.size(); i += 2)
					{
						if (stoi(acceptedData[i]) >= FIRST_FLOOR && stoi(acceptedData[i + 1]) >= FIRST_FLOOR)
						{
							*index = passengerSet.size();
							passengerSet.resize(passengerSet.size() + 1, vector<int>(INFO_P));
							passengerSet[*index][core::go::num] = ++(*passengerNum);
							passengerSet[*index][core::go::from] = stoi(acceptedData[i]);
							passengerSet[*index][core::go::to] = stoi(acceptedData[i + 1]);
							passengerSet[*index][core::go::status] == core::pStatus::added;
							passengerSet[*index][core::go::direction] == func.getDirection(stoi(acceptedData[i]), stoi(acceptedData[i + 1]));
							passengerSet[*index][core::go::elevator] = (FIRST_ELEVATOR - 1);
							assignElevator(*index);
						}
					}
					vector<string>().swap(acceptedData);
					delete index;
					return true;
				}
			}
			return false;
		}
		catch (const exception & e)
		{
			func.msgError(e, "core_addPassenger");
		}
	}

	vector<string> core::isValidPassenger(vector<string> data)
	{
		vector<string> dt;
		if (data.size() > 0 && data.size() % UNIT == 0)
		{
			for (int i = 0; i < data.size(); i += 2)
			{
				if (!func.isValue(data[i].c_str()))
					continue;
				if (!func.isValue(data[i + 1].c_str()))
					continue;
				if ((stoi(data[i]) >= FIRST_FLOOR && stoi(data[i]) <= (*floorNums)) && \
					(stoi(data[i + 1]) >= FIRST_FLOOR && stoi(data[i + 1]) <= (*floorNums)))
				{
					dt.push_back(data[i]);
					dt.push_back(data[i + 1]);
				};
			}
		}
		return dt;
	}

	int8_t core::isValidHeader(const vector<string>& data)
	{
		if (data.size() == HEAD)
		{
			if (!func.isValue(data[core::header::floors].c_str()))
				return false;
			if (!func.isValue(data[core::header::elevators].c_str()))
				return false;
			if (!func.isValue(data[core::header::secToMove].c_str()))
				return false;
			if (!func.isValue(data[core::header::secOnOff].c_str()))
				return false;
			if (!func.isValue(data[core::header::passengers].c_str()))
				return false;
			*floorNums = stoi(data[core::header::floors]);
			*elevatorNums = stoi(data[core::header::elevators]);
			*timeTakenToMove = stoi(data[core::header::secToMove]);
			*timeTakenOnOff = stoi(data[core::header::secOnOff]);
			*passenserLimit = stoi(data[core::header::passengers]);

			if (*floorNums >= FIRST_FLOOR && *elevatorNums >= FIRST_ELEVATOR && \
				*timeTakenToMove >= MIN_TIME_COST && *timeTakenOnOff >= MIN_TIME_COST)
			{
				for (int32_t i = 1; i <= *elevatorNums; i++)
				{
					elevatorMap[i].floor = core::eFlow::checkBoarded;
					elevatorMap[i].floor = 1;
					elevatorMap[i].status = core::eStatus::stop;
					elevatorMap[i].sum = 0;
					elevatorMap[i].attempts = 0;
					elevatorMap[i].lastStop = (FIRST_FLOOR - 1);
				}
				return READY_HEADER;
			}
		}
		return READY_PROGRAMME;
	}

	void core::assignElevator(const int16_t& passengerIndex)
	{
		try
		{
			int16_t* matched = new (nothrow) int16_t;
			int16_t* startIdx = new (nothrow) int16_t;
			int32_t* from = new (nothrow) int32_t;
			int32_t* to = new (nothrow) int32_t;
			unsigned long long* minSum = new (nothrow) unsigned long long;
			unsigned long long* sum = new (nothrow) unsigned long long;
			unsigned long long* cost = new (nothrow) unsigned long long;
			int32_t* minIndex = new (nothrow) int32_t;
			int32_t* tempVal = new (nothrow) int32_t;
			int32_t* fromCount = new (nothrow) int32_t;

			if (!matched) new (nothrow) int16_t;
			if (!matched) new (nothrow) int16_t;
			if (!from) new (nothrow) int32_t;
			if (!to) new (nothrow) int32_t;
			if (!minSum) new (nothrow) unsigned long long;
			if (!sum) new (nothrow) unsigned long long;
			if (!cost) new (nothrow) unsigned long long;
			if (!minIndex) new (nothrow) int32_t;
			if (!tempVal) new (nothrow) int32_t;
			if (!fromCount) new (nothrow) int32_t;
			if (matched && startIdx && from && to && minSum && sum && cost && minIndex && tempVal && fromCount)
			{
				*minSum = (INT32_MAX);
				*minIndex = (FIRST_ELEVATOR - 1);
				*from = passengerSet[passengerIndex][core::go::from];
				*to = passengerSet[passengerIndex][core::go::to];
				passengerSet[passengerIndex][core::go::direction] = func.getDirection(*from, *to);
				if (*minIndex == (FIRST_ELEVATOR - 1) && elevatorMap.size() == 1)
				{
					*minIndex = elevatorMap.begin()->first;
				}
				else
				{
					for (int32_t i = 1; i <= elevatorMap.size(); i++)
					{
						if (func.abs(elevatorMap[i].sum - elevatorMap[i].alighting[elevatorMap[i].floor]) >= *passenserLimit)
						{
							continue;
						}
						if (elevatorMap[i].direction == passengerSet[passengerIndex][core::go::direction])
						{
							switch (elevatorMap[i].direction)
							{
							case DIRECTION_CLOCK: // downward
								if (elevatorMap[i].floor < passengerSet[passengerIndex][core::go::from])
									continue;
							case DIRECTION_COUNTER: // upward
								if (elevatorMap[i].floor > passengerSet[passengerIndex][core::go::from])
									continue;
							}
						}
						*cost = 0;
						*startIdx = 0;
						map<int16_t, vector<int16_t>> fromArray;
						vector<vector<int16_t>> toArray;
						vector<vector<int16_t>> tempArray;
						fromArray[passengerSet[passengerIndex][core::go::from]].push_back(passengerSet[passengerIndex][core::go::to]);
						while ((*matched = findEleToPpl(i, *startIdx)) != (end(passengerSet) - begin(passengerSet)))
						{
							*startIdx = (*matched + 1);
							fromArray[passengerSet[*matched][core::go::from]].push_back(passengerSet[*matched][core::go::to]);
						}
						// Compare the values with its arrival and the next arrival floors
						// and insert between them if the value(s) is in case as given below.
						*fromCount = elevatorMap[i].floor;
						for (auto k : fromArray)
						{
							tempArray.push_back(vector<int16_t>());
							tempArray[tempArray.size() - 1].resize(2);
							tempArray[tempArray.size() - 1][0] = (k.first);
							tempArray[tempArray.size() - 1][1] = (k.second.size());
							for (int j = 0; j < fromArray[k.first].size(); j++)
							{
								if (fromArray[k.first][j] > *fromCount && fromArray[k.first][j] < prev(fromArray.end())->first)
								{
									// Push the value in ascending order.
									tempArray.push_back(vector<int16_t>());
									tempArray[tempArray.size() - 1].resize(2);
									tempArray[tempArray.size() - 1][0] = (fromArray[k.first][j]);
									tempArray[tempArray.size() - 1][1] = (fromArray[k.first].size());
									(*cost) += ((unsigned long long)(tempArray[tempArray.size() - 1][1]) * (*timeTakenOnOff));
								}
								else
								{
									toArray.push_back(vector<int16_t>());
									toArray[toArray.size() - 1].resize(2);
									toArray[toArray.size() - 1][0] = (fromArray[k.first][j]);
									toArray[toArray.size() - 1][1] = (fromArray[k.first].size());
									(*cost) += ((unsigned long long)(toArray[toArray.size() - 1][1]) * (*timeTakenOnOff));
								}
							}
							*fromCount = k.first;
						}
						if (fromArray.size() > 0)
						{
							(*cost) += ((unsigned long long)func.abs(elevatorMap[i].floor - tempArray[0][0]) * (*timeTakenToMove));
							(*cost) += ((unsigned long long)func.abs(tempArray[0][0] - tempArray[tempArray.size() - 1][0]) * (*timeTakenToMove));
							if (toArray.size() > 0)
							{
								(*cost) += ((unsigned long long)func.abs(tempArray[tempArray.size() - 1][0] - toArray[toArray.size() - 1][0]) * (*timeTakenToMove));
								(*cost) += ((unsigned long long)func.abs(toArray[toArray.size() - 1][0] - toArray[0][0]) * (*timeTakenToMove));
							}
						}
						// Insert the remaining values in descending order.
						// Update both minSum and minIndex based on the given lowest value.
						if (*cost < *minSum)
						{
							*minSum = *cost;
							*minIndex = i;
						}
						fromArray.clear();
						toArray.clear();
						tempArray.clear();
						toArray.shrink_to_fit();
						tempArray.shrink_to_fit();
					}
				}
				// Update the values of the elements of both arrays of passenger and elevator
				// when a different optimal target has been selected.
				if (*minIndex >= FIRST_ELEVATOR)
				{
					passengerSet[passengerIndex][core::go::elevator] = *minIndex;
					elevatorMap[*minIndex].direction = passengerSet[passengerIndex][core::go::direction];
					elevatorMap[*minIndex].sum++;
					if (elevatorMap[*minIndex].boarded.find(*from) == elevatorMap[*minIndex].boarded.end())
					{
						elevatorMap[*minIndex].boarded[*from] = 1;
					}
					else
					{
						elevatorMap[*minIndex].boarded[*from]++;
					}
					if (elevatorMap[*minIndex].alighting.find(*to) == elevatorMap[*minIndex].alighting.end())
					{
						elevatorMap[*minIndex].alighting[*to] = 1;
					}
					else
					{
						elevatorMap[*minIndex].alighting[*to]++;
					}
				}
				delete from;
				delete to;
				delete minSum;
				delete sum;
				delete cost;
				delete minIndex;
				delete tempVal;
				delete startIdx;
				delete matched;
				delete fromCount;
			}
		}
		catch (const exception & e)
		{
			func.msgError(e, "core_isValidPassenger");
		}
	}

	int32_t core::findWaitingPpl(int32_t startIdx)
	{
		try
		{
			auto it = find_if(begin(passengerSet) + startIdx, end(passengerSet),
				[&](vector<int32_t> const& v)
				{ return v[core::go::status] == core::pStatus::added; });
			return (it - begin(passengerSet));
		}
		catch (const exception & e)
		{
			func.msgError(e, "core_findWaitingPpl");
		}
	}

	int32_t core::findEleToPpl(int32_t eleIdx, int32_t startIdx)
	{
		try
		{
			auto it = find_if(begin(passengerSet) + startIdx, end(passengerSet),
				[&](vector<int32_t> const& v)
				{ return v[core::go::elevator] == eleIdx; });
			return (it - begin(passengerSet));
		}
		catch (const exception & e)
		{
			func.msgError(e, "core_findEleToPpl");
		}
	}

	int32_t core::findStartEndFloorToPpl(int32_t floor, int32_t startIdx)
	{
		try
		{
			auto it = find_if(begin(passengerSet) + startIdx, end(passengerSet),
				[&](vector<int32_t> const& v)
				{ return v[core::go::from] == floor || v[core::go::to] == floor; });
			return (it - begin(passengerSet));
		}
		catch (const exception & e)
		{
			func.msgError(e, "core_findStartEndFloorToPpl");
		}
	}

	int32_t core::findStartFloorToPpl(int32_t floor, int32_t startIdx)
	{
		try
		{
			auto it = find_if(begin(passengerSet) + startIdx, end(passengerSet),
				[&](vector<int32_t> const& v)
				{ return v[core::go::from] == floor; });
			return (it - begin(passengerSet));
		}
		catch (const exception & e)
		{
			func.msgError(e, "core_findStartFloorToPpl");
		}
	}

	int32_t core::findEndFloorToPpl(int32_t floor, int32_t startIdx)
	{
		try
		{
			auto it = find_if(begin(passengerSet) + startIdx, end(passengerSet),
				[&](vector<int32_t> const& v)
				{ return v[core::go::to] == floor; });
			return (it - begin(passengerSet));
		}
		catch (const exception & e)
		{
			func.msgError(e, "core_findEndFloorToPpl");
		}
	}
}