#pragma once
#include <vector>
#include <map>

namespace NCL {
	namespace CSC8503 {

		//predeclare the classes we need
		class State;
		class StateTransition;

		//typedefs to make life easier!
		typedef std::multimap<State*, StateTransition*> TransitionContainer;
		typedef TransitionContainer::iterator TransitionIterator;

		class StateMachine	{
		public:
			StateMachine();
			~StateMachine();

			void AddState(State* s);
			void AddTransition(StateTransition* t);

			void Update();

		protected:
			State * activeState;

			std::vector<State*> allStates;

			TransitionContainer allTransitions;
		};
	}
}