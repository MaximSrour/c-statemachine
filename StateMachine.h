#pragma once

#include <map>

/**
 * Defines the return status of the state machine
 * It can alert the program if the owner is null
 * It can also track whether a tick function or state change has occured
 */
enum EStateMachineStatus {
	NullOwner,
	StateChanged,
	StateUnchanged,
	TickSuccess,
};

/**
 * An implementation of a state machine that allows for the triggering of function delegates on state change
 * @tparam EnumClass The user defined enum/index for a unique state
 * @tparam TemplateClass The class that owns the state machine
 */
template<class EnumClass, class TemplateClass>
class TStateMachine {
public:
	typedef void (TemplateClass::*CallbackOnEnter)();
	typedef void (TemplateClass::*CallbackOnUpdate)(float);
	typedef void (TemplateClass::*CallbackOnExit)();

private:
	TemplateClass* Owner;
	EnumClass CurrentState;

	/** Callback function delegates map */
	std::map<EnumClass, CallbackOnEnter> StatesOnEnter;
	std::map<EnumClass, CallbackOnUpdate> StatesTick;
	std::map<EnumClass, CallbackOnEnter> StatesOnExit;

public:
	/**
	 * Constructor that takes the owner of the state machine and the enum class
	 * @param owner Template of the object that owns the state machine
	 * @param emptyState Enum class that is being tracked against
	 */
	TStateMachine(TemplateClass* owner, EnumClass emptyState) {
		Owner = owner;
		CurrentState = emptyState;
	}

	/**
	 * Default desctructor
	 */
	~TStateMachine() {

	}

	/**
	 * Adds a state to the state machine with the relevant function delegates
	 * @param state The new state to be added
	 * @param callbackEntry The function delegate to be executed when entering the state
	 * @param callbackEntry The function delegate to be executed on tick when in the state
	 * @param callbackEntry The function delegate to be executed when exiting the state
	 */
	void RegisterState(EnumClass state, CallbackOnEnter callbackEntry, CallbackOnUpdate callbackTick, CallbackOnExit callbackExit) {
		StatesOnEnter[state] = callbackEntry;
		StatesTick[state] = callbackTick;
		StatesOnExit[state] = callbackExit;
	}

	/**
	 * Gets the current state of the state machine
	 * @returns The current state
	 */
	EnumClass GetCurrentState() {
		return CurrentState;
	}

	/**
	 * Change the state machine to a different state and execute Enter/Exit delegates
	 * Only changes state if the new state is different, hence it won't run the delegates if the new state is the same
	 * @param stateNext The new state to be switched to
	 * @return State machine status to indicate any failures
	 */
	EStateMachineStatus ChangeState(EnumClass stateNext) {
		if(Owner) {
			//Only switch state if it the new one is different to the old
			//This stops the exit and enter delegates from being run if not needed
			if(CurrentState != stateNext) {
				//Run exit delegate
				CallbackOnExit callbackExit = StatesOnExit[CurrentState];
				if(callbackExit) {
					(Owner->*callbackExit)();
				}

				//Set state
				CurrentState = stateNext;

				//Run enter delegate
				CallbackOnEnter callbackEnter = StatesOnEnter[CurrentState];
				if(callbackEnter) {
					(Owner->*callbackEnter)();
				}

				return EStateMachineStatus::StateChanged;
			}

			return EStateMachineStatus::StateUnchanged;
		}

		return EStateMachineStatus::NullOwner;
	}

	/**
	 * Tick the state machine using the tick delegate that is relevant to the state
	 * @param DeltaTime Is the time elapsed between frames
	 * @return State machine status to indicate any failures
	 */
	EStateMachineStatus Tick(float DeltaTime) {
		if(Owner) {
			CallbackOnUpdate callback = StatesTick[CurrentState];
			if(callback) {
				(Owner->*callback)(DeltaTime);

				return EStateMachineStatus::TickSuccess;
			}
		}

		return EStateMachineStatus::NullOwner;
	}
};
