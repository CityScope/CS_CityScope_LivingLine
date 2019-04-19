using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using BehaviorDesigner.Runtime;
using BehaviorDesigner.Runtime.Tasks;
using Pathfinding;

[TaskCategory("Game")]
public class RemoveAIAction : Action
{
    public override void OnAwake()
    {
        base.OnAwake();
        SingletonTMono<App>.Instance.RemoveAI(this.gameObject);
    }
}
