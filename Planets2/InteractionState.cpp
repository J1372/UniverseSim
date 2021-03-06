#include "InteractionState.h"

#include "DefaultInteraction.h"
#include "PlanetCreation.h"
#include "SystemCreation.h"

DefaultInteraction InteractionState::default_interaction;
PlanetCreation InteractionState::planet_interaction;
SystemCreation InteractionState::system_interaction;

void InteractionState::init_states()
{
	default_interaction = {};
	planet_interaction = {};
	system_interaction = {};
}
