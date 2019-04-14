using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using BehaviorDesigner.Runtime;
using BehaviorDesigner.Runtime.Tasks;
using Pathfinding;

[TaskCategory("AIAnimation/Animation")]
public class AIMoveTargetPointAction : Action
{
    IAstarAI ai;
    public SharedTransform target;


    public override void OnAwake()
    {
        base.OnAwake();
        ai = this.transform.GetComponent<IAstarAI>();
    }

    public override TaskStatus OnUpdate()
    {
       

        if (target != null && ai != null) ai.destination = target.Value.position;

        return base.OnUpdate();
    }

    void GetPoint()
    {

    }
}
