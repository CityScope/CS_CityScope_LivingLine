using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Newtonsoft.Json;

public class UrbanAnalytics : MonoBehaviour
{
    public bool useUDP = true;
    public string jsonData;
    public UdpListener udpListener;
    public RadarChart radarChart;
    public BarChart barChart;

    public int fixedUnitTotalTypeNum = 7;
    public float[] fixedUnitCapacities;
    public int[] fixedUnitNumbers;

    void Start()
    {

        // if use udp
        if (useUDP)
        {
            if (udpListener._encodedUDP == "")
            {
                Debug.LogWarning("UDP recieved null");
            }
            else
            {
                jsonData = udpListener._encodedUDP;
            }
        }

        // sample data
        // { "fixed_units": [{"type": 3, "x": 219.31, "y": 54.66}, ...
        // ... {"type": 6, "x": 113.88, "y": 93.92}], 
        // "free_units": [{"type": 8, "x": 185.51, "y": 91.03, "rot": 278.6}, ...
        // ... {"type": 8, "x": 9.38, "y": 103.15, "rot": 329.62}], 
        // "knobs": [{"type": 10, "x": 276.52, "y": 10.55, "rot": 274.2}, 
        // {"type": 11, "x": 290.22, "y": 106.35, "rot": 161.43}, 
        // {"type": 12, "x": 318.57, "y": 112.41, "rot": 122.77}]}

        // initiate fixed unit capacity list
        fixedUnitCapacities = new float[fixedUnitTotalTypeNum];
        for (int i = 0; i < fixedUnitTotalTypeNum; i++)
        {
            fixedUnitCapacities[i] = 1.0f;
        }

        // initiate fixed unit number list
        fixedUnitNumbers = new int[fixedUnitTotalTypeNum];
        for (int i = 0; i < fixedUnitTotalTypeNum; i++)
        {
            fixedUnitNumbers[i] = 0;
        }

    }

    void Update()
    {
        if (useUDP)
        {
            if (udpListener._encodedUDP == "")
            {
                Debug.LogWarning("UDP recieved null");
            }
            else
            {
                jsonData = udpListener._encodedUDP;

                JsonData data = JsonConvert.DeserializeObject<JsonData>(jsonData);

                UpdateRadarChart(data);
                UpdateBarChart(data);
                UpdateHeatmap(data);
            }
        }
    }

    void UpdateRadarChart(JsonData jsonData)
    {
        // RZ TODO: need to be fixed, now counting the fixed unit on the side panel
        // calculate fixed units numbers for each type
        for (int i = 0; i < fixedUnitTotalTypeNum; i++)
        {
            fixedUnitNumbers[i] = 0;
        }
        foreach (UnitInfoData infoData in jsonData.fixed_units)
        {
            if (infoData.type < fixedUnitTotalTypeNum)
            {
                fixedUnitNumbers[infoData.type]++;
            }
        }
    }

    void UpdateBarChart(JsonData jsonData)
    {

    }

    void UpdateHeatmap(JsonData jsonData)
    {

    }
}
