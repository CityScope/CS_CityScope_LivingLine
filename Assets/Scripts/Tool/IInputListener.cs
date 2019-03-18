using System;
using System.Collections.Generic;
using UnityEngine;

//����ӿ�
public interface IInputListener
{
    //���������б�
	void updateInputs(List<AYInput> ayInputList);
    //��Ļ����
	void inputScreenMove(Vector2 delta,Vector2 totalDelta, AYInput ayInput);
    //���廬��
    void inputWorldMove(Vector2 delta, Vector2 totalDelta, AYInput ayInput);
    //����
    void OnPressed(bool pressed);
}