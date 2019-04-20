using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using BehaviorDesigner.Runtime;
using BehaviorDesigner.Runtime.Tasks;
using Pathfinding;

[TaskCategory("Game")]
public class AIMoveEventAction : Action
{
    IAstarAI ai;
    public SharedTransform target;
    public SharedFloat stopDistance;


    public override void OnAwake()
    {
        base.OnAwake();
        ai = this.transform.GetComponent<IAstarAI>();

    }

    public override TaskStatus OnUpdate()
    {
       if(target==null)
        {
            return TaskStatus.Failure;
        }

        if (target != null && ai != null) ai.destination = target.Value.position;

        if (ai.reachedEndOfPath || ai.remainingDistance < stopDistance.Value)
        {
            ai.isStopped = true;
            return TaskStatus.Success;
        }

        return TaskStatus.Running;

    }
}
