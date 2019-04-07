using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class FixedUnit : MonoBehaviour
{
    public Fixed_UnitData data;
    public Vector3 maxPos;
    public Vector3 minPos;

    void Start()
    {
        
    }

    public void Show(UnitInfoData infoData,Vector3 offset)
    {
        if(maxPos== Vector3.zero&&minPos == Vector3.zero)
        {
            Debug.LogError("Unit All is zero..");
            return;
        }

        if((maxPos.x == 0||infoData.x <maxPos.x+offset.x&&infoData.x > minPos.x + offset.x) && (maxPos.y == 0 || infoData.y<maxPos.y + offset.y&& infoData.y>minPos.y + offset.y))
        {
            gameObject.SetActive(true);
        }
    }
}
