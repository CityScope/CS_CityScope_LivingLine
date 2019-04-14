using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Linq;
using UnityEngine.UI;

public class AxisScoreCtrl : MonoBehaviour {
    
    //public GameObject orange;
    public CityMatrixRadarChart CMRadarChart;
    public Text[] currentAxises;
    //public Text[] suggestedAxises;
    //public GameObject currentScores;
    //public GameObject suggestedScores;
    //public GameObject arrows;
    //public bool showAISuggestion;
    
    void Update () {

        // current
        for (int i = 0; i < currentAxises.Length; i ++)
        {
            if(CMRadarChart!=null&&CMRadarChart.metrics!=null)
            {
                currentAxises[i].text = string.Format("{0:0}", CMRadarChart.metrics[i] * 100f);
            }
            
        }

    }
}
