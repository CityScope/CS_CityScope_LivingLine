using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PathPoint : Point
{
    
    public enum PathPointType
    {
       Work,
       Residence
    }

    public PathPointType pathPointType;
    public string Id;

}
