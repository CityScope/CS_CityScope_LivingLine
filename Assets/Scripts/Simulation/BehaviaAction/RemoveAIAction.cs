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
       
    }

    public override void OnStart()
    {
        base.OnStart();
        SingletonTMono<App>.Instance.RemoveAI(this.gameObject);
        SharedGameObject go = (SharedGameObject)this.Owner.GetVariable("headUI");
        if(go!=null)
        {

        }
    }
}
