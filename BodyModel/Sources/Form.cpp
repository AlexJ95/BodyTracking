#include "Form.h"

Form::Form()
{

}

bool Form::isFormShown()
{
	return showingForm;
}

bool Form::gameStarted()
{
	return startGame;
}

//will overwrited in the derived class
void Form::drawForm()
{

}


