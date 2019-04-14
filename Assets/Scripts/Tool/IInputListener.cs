using System;
using System.Collections.Generic;
using UnityEngine;

//输入接口
public interface IInputListener
{
    //输入数据列表
	void updateInputs(List<AYInput> ayInputList);
    //屏幕滑动
	void inputScreenMove(Vector2 delta,Vector2 totalDelta, AYInput ayInput);
    //物体滑动
    void inputWorldMove(Vector2 delta, Vector2 totalDelta, AYInput ayInput);
    //按下
    void OnPressed(bool pressed);
}