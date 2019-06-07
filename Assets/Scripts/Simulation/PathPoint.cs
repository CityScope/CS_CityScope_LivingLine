using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PathPoint : Point_Sim
{
    public enum PathPointType
    {
       Work,
       Residence,
       Exit,
       Enter,
       custum,
    }

    public PathPointType pathPointType;
    public string Id;

}
