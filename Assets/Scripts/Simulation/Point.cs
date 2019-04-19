using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Point : MonoBehaviour
{
    public float radius = 1;
    public Color color = Color.yellow;
    
    
    void Start()
    {
        
    }

    void OnDrawGizmos()
    {

        Gizmos.color = color;
        Gizmos.DrawWireSphere(transform.position, radius);
    }

}
