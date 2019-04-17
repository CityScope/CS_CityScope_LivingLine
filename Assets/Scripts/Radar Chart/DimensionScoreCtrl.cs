using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Linq;
using UnityEngine.UI;

public class DimensionScoreCtrl : MonoBehaviour {
    
    public RadarChart radarChart;
    public Text[] currentDimensions;
    
    void Update () {

        // current
        for (int i = 0; i < currentDimensions.Length; i ++)
        {
            currentDimensions[i].text = string.Format("{0:0}", radarChart.dimensionMetrics[i] * 100f);
        }

    }
}
