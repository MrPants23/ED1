#pragma once

#include "../EDGOCA/BehaviorT.h"

using namespace EDGOCA;

namespace Behaviors
{
	class TranslatorBehavior : public BehaviorT<TranslatorBehavior>
	{
	public:
		TranslatorBehavior(void)
		{
			baseSpeed = 7.5f;
		}

		~TranslatorBehavior(void) {}

		static void PopulateMessageMap(void);

		static void OnMoveForward( IBehavior* invokingBehavior, IMessage* message );
		static void OnMoveBackward( IBehavior* invokingBehavior, IMessage* message );
		static void OnMoveLeft( IBehavior* invokingBehavior, IMessage* message );
		static void OnMoveRight( IBehavior* invokingBehavior, IMessage* message );
		static void OnMoveUp( IBehavior* invokingBehavior, IMessage* message );
		static void OnMoveDown( IBehavior* invokingBehavior, IMessage* message );

		static void OnMoveForwardSolution( IBehavior* invokingBehavior, IMessage* message );
		static void OnMoveBackwardSolution( IBehavior* invokingBehavior, IMessage* message );
		static void OnMoveLeftSolution( IBehavior* invokingBehavior, IMessage* message );
		static void OnMoveRightSolution( IBehavior* invokingBehavior, IMessage* message );
		static void OnMoveUpSolution( IBehavior* invokingBehavior, IMessage* message );
		static void OnMoveDownSolution( IBehavior* invokingBehavior, IMessage* message );


	private:
		float baseSpeed;

		float ComputeSpeed();
	};
}
