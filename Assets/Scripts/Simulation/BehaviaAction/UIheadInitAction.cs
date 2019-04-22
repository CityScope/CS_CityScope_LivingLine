using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using BehaviorDesigner.Runtime;
using BehaviorDesigner.Runtime.Tasks;
using Pathfinding;

[TaskCategory("Game")]
public class UIheadInitAction : Action
{

    public string HeadPointName;

    public override void OnAwake()
    {

        base.OnAwake();
        return;
        GameObject go = Tool.GetChildInDepth(HeadPointName, this.gameObject);
        if (go == null)
        {
            return;
        }
        GameObject aiHeadRoot = GameObject.Find("AIHeadRoot");

        if (aiHeadRoot == null)
            return;
        GameObject headPrefab = GameObject.Instantiate<GameObject>(Tool.GetChildInDepth("HeadPrefab", aiHeadRoot));
        headPrefab.transform.parent = aiHeadRoot.transform;
        headPrefab.transform.localPosition = Vector3.zero;
        headPrefab.transform.localScale = Vector3.one;
        headPrefab.transform.localEulerAngles = Vector3.zero;
        headPrefab.SetActive(true);
        UIFollowObj uiFollow = go.AddComponent<UIFollowObj>();
        uiFollow.SetParam(headPrefab.GetComponent<RectTransform>());
        SharedGameObject sharedGO = new SharedGameObject();
        sharedGO.Value = headPrefab;
        this.Owner.SetVariable("headUI", sharedGO);

    }

}
