using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using BehaviorDesigner.Runtime;
using BehaviorDesigner.Runtime.Tasks;
using Pathfinding;

[TaskCategory("Game")]
public class AIMoveTargetPointAction : Action
{
    IAstarAI ai;
    public SharedTransform target;
    public PathPoint.PathPointType pointType;
    public float stopDistance = 3;
    public SharedBool stopMoveFlag;
    public bool ignoreStopFlag;

    public override void OnAwake()
    {
        base.OnAwake();
        ai = this.transform.GetComponent<IAstarAI>();
        //stopMoveFlag = (SharedBool)this.Owner.GetVariable("StopAiMove");
    }

    public override void OnStart()
    {
        base.OnStart();
        if (this.stopMoveFlag != null && this.stopMoveFlag.Value)
        {
            
        }else 
        {
            GetTarget();
        }
       
        ai.isStopped = false;
    }

    public override TaskStatus OnUpdate()
    {
        if (this.stopMoveFlag != null && this.stopMoveFlag.Value&& !ignoreStopFlag)
        {
            //ai.isStopped = true;
            return TaskStatus.Running;
        }
        if (target != null && ai != null) ai.destination = target.Value.position;
        //Debug.Log("Distance..." + ai.remainingDistance);
        if (ai.reachedEndOfPath || ai.remainingDistance < stopDistance)
        {
            ai.isStopped = true;
            return TaskStatus.Success;
        }

        if(target == null)
        {
            GetTarget();
        }

        ai.isStopped = false;
        return TaskStatus.Running;
    }

    void GetTarget()
    {

        PathPoint point = SingletonTMono<App>.Instance.GetRandomPoint(pointType);
        if (pointType != PathPoint.PathPointType.custum)
        {
            if (point != null)
            {
                target.Value = point.transform;
            }

        }

        
    }
}
