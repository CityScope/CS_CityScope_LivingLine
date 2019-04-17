using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class LivingLineBarChart : MonoBehaviour
{
    [Range(0, 2)]
    public int vizType = 0;
    [Range(0, 1f)]
    public float[] metrics;

    public GameObject barsRoot;
    public GameObject barSource;
    public string barGoName = "bar profile 0";
    public GameObject[] bars;

    // Update is called once per frame
    void Update()
    {
        
    }
}
