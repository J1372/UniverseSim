#include "InteractionState.h"

#include "DefaultInteraction.h"
#include "PlanetCreation.h"
#include "SystemCreation.h"

DefaultInteraction InteractionState::default_interaction;
PlanetCreation InteractionState::planet_interaction;
SystemCreation InteractionState::system_interaction;

void InteractionState::init_states()
{
	default_interaction.init();
	planet_interaction.init();
	system_interaction.init();
}
