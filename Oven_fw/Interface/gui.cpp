/*
 * gui.cpp
 *
 *  Created on: 13 ??? 2016 ?.
 *      Author: Kreyl
 */

#include "gui.h"

const Button_t BtnOk {50, 100, 50, 50, "OK"};
const Button_t BtnCancel {50, 100, 50, 50, "OK"};

const Widget_t Page0Widgets[2] = { &BtnOk, &BtnCancel };


const Page_t Page0 { 2, Page0Widgets };
