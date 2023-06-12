using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UrbanAnalytics : MonoBehaviour
{
    public bool useUDP = true;
    public string jsonData;
    public UdpListener udpListener;
    public RadarChart radarChart;
    public BarChart barChart;
    
    public float[] fixedUnitCapacities = new float[7];
    public int[] fixedUnitNumbers = new int[7];

    public float freeUnitCapacityMin = 0.1f;
    public float freeUnitCapacityMax = 30.0f;

    public float[] timeFactors = new float[6]; // 6 bars
    public TimeKnob timeKnob;
    public float timeFactor;

    public Heatmap heatmap;
    public float heatmapRangeFactorFixedUnit = 25f;
    public float heatmapRangeFactorFreeUnit = 50f;
    public float heatmapIntensityFactorFixedUnit = 0.5f;
    public float heatmapIntensityFactorFreeUnit = 1.0f;

    [Range(0.001f, 10.0f)]
    public float metricsFactor = 1.0f;
    public float barMetricsFactor = 0.5f;
    public float[,] weightMatrix = new float[9, 10] {  // row: 9 metrics; col: 10 types; Chart: https://docs.google.com/spreadsheets/d/1Px9YKB4KISgxatSMCHH4DWIWQqlYVx67bZ3H0zxhjQc/edit?usp=sharing (tab: trial_for Unity)
        {50f, 20f, 10f, 8f, 2f, 10f, 5f, 15f, 4f, 5f},
        {10f, 20f, 5f, 0f, 0f, 20f, 10f, 20f, 10f, 10f},
        {20f, 30f, 10f, 5f, 5f, 25f, 10f, 8f, 2f, 10f},
        {0.9f, 0.95f, 0.4f, 0.3f, 0.2f, 0.9f, 0.6f, 0.6f, 0.3f, 0.4f},
        {0.5f, 0.5f, 0.3f, 0.2f, 0.2f, 0.6f, 0.4f, 0.3f, 0.2f, 0.3f},
        {0.4f, 0.65f, 0.5f, 0.5f, 0.2f, 0.7f, 0.4f, 0.3f, 0.4f, 0.7f},
        {0.4f, 0.6f, 0.4f, 0.3f, 0.2f, 0.7f, 0.3f, 0.4f, 0.5f, 0.6f},
        {1000f, 750f, 100f, 28f, 10f, 400f, 100f, 150f, 45f, 50f},
        {190f, 400f, 45f, 20f, 20f, 45f, 40f, 30f, 5f, 45}
    };
    // RZ: BK Weiting 190401 original
    // https://docs.google.com/spreadsheets/d/1Px9YKB4KISgxatSMCHH4DWIWQqlYVx67bZ3H0zxhjQc/edit?usp=sharing (tab: RZ: for Unity)
    /*  
        {15f, 10f, 10f, 8f, 2f, 10f, 5f, 10f, 4f, 5f},
        {5f, 15f, 5f, 0f, 0f, 20f, 10f, 15f, 10f, 10f},
        {20f, 15f, 10f, 5f, 0f, 10f, 10f, 8f, 2f, 10f},
        {0.87f, 0.98f, 0.72f, 0.34f, 0.2f, 0.96f, 0.79f, 0.67f, 0.65f, 0.35f},
        {0.66f, 0.85f, 0.54f, 0.42f, 0.26f, 0.85f, 0.73f, 0.73f, 0.71f, 0.57f},
        {0.55f, 0.65f, 0.51f, 0.57f, 0.26f, 0.79f, 0.65f, 0.35f, 0.72f, 0.67f},
        {0.5f, 0.63f, 0.43f, 0.69f, 0.27f, 0.74f, 0.81f, 0.46f, 0.58f, 0.73f},
        {95f, 150f, 52f, 28f, 1f, 217f, 52f, 150f, 45f, 52f},
        {45f, 105f, 45f, 10f, 5f, 45f, 45f, 28f, 5f, 45}
    */
    public float[] metricsMax = new float[9] { 100f, 100f, 100f, 10f, 10f, 10f, 10f, 1000f, 1000f };
    public float[] metricsMin = new float[9] { 0f, 0f, 0f, 0f, 0f, 0f, 0f, 0f, 0f };

    public float[] allUnitTotalCapacities = new float[10];

    public float[] metrics = new float[9];

    // RZ 190515 temp fix for filming
    // public App app;

    void Start()
    {

        // if use udp
        if (useUDP)
        {
            if (udpListener._encodedUDP == "")
            {
                //Debug.LogWarning("UDP recieved null");
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

        // print weight matrix
        int rowLength = weightMatrix.GetLength(0);  // 9 (metrics)
        int colLength = weightMatrix.GetLength(1);  // 10 (unit types)
        Debug.Log(string.Format("rowLength = {0}", rowLength));
        Debug.Log(string.Format("colLength = {0}", colLength));
        string print = "";
        for (int i = 0; i < rowLength; i++)
        {
            for (int j = 0; j < colLength; j++)
            {
                print += string.Format("{0} ", weightMatrix[i, j]);
            }
            print += "\n";
        }
        Debug.Log(print);

    }

    void Update()
    {
        if (useUDP)
        {
            if (udpListener._encodedUDP == "")
            {
                //Debug.LogWarning("UDP recieved null");
            }
            else
            {
                jsonData = udpListener._encodedUDP;
                
                JsonData data = Newtonsoft.Json.JsonConvert.DeserializeObject<JsonData>(jsonData);

                UpdateRadarChart(data);
                UpdateBarChart(data);
                UpdateHeatmap(data);
            }
        }
    }

    void UpdateRadarChart(JsonData jsonData)
    {
        // Computing fixed units' total capacities (number * capacities)
        // numbers for each type
        // RZ TODO: need to be fixed, now counting the fixed unit on the side panel
        for (int i = 0; i < 7; i++)
        {
            fixedUnitNumbers[i] = 0;
        }
        foreach (UnitInfoData infoData in jsonData.fixed_units)
        {
            if (infoData.type < 7)
            {
                fixedUnitNumbers[infoData.type]++;
            }
        }
        // calculate total capacities for fixed units
        for (int i = 0; i < 7; i++)
        {
            allUnitTotalCapacities[i] = fixedUnitNumbers[i] * fixedUnitCapacities[i];
        }


        // Computing free units' total capacities (number * capacities)
        for (int i = 7; i < 10; i++)
        {
            allUnitTotalCapacities[i] = 0.0f;
        }
        // RZ 190515 temp fix for filming
        /*
        foreach (UnitInfoData infoData in jsonData.free_units)
        {
            if (infoData.type >= 7 && infoData.type < 10)
            {
                allUnitTotalCapacities[infoData.type] += FreeUnitRot2Capacity(infoData.rot);
            }
        }
        */
        int t = 7;
        // foreach (GameObject freeUnitGO in app.freeUnits)
        // {
        //     allUnitTotalCapacities[t] += FreeUnitRot2Capacity(-freeUnitGO.transform.localEulerAngles.y);
        //     t++;
        // }


        // Computing all units' impact on radar chart metrics
        metrics = new float[9]; // { 0f, 0f, 0f, 0f, 0f, 0f, 0f, 0f, 0f };
        int rowLength = weightMatrix.GetLength(0);  // 9 (metrics)
        int colLength = weightMatrix.GetLength(1);  // 10 (unit types)
        string print = "";
        for (int i = 0; i < rowLength; i++)
        {
            for (int j = 0; j < colLength; j++)
            {
                // row: i: 9 metrics; col: j; 10 types;
                metrics[i] += weightMatrix[i, j] * allUnitTotalCapacities[j];
            }
            print += metrics[i] + ", ";
        }
        //Debug.Log("matrics(raw): " + print);

        // computer normalized metrics;
        print = "";
        for (int i = 0; i < metrics.Length; i++)
        {
            metrics[i] = (metrics[i] - metricsMin[i]) / (metricsMax[i] - metricsMin[i]) * metricsFactor;
            print += metrics[i] + ", ";
        }
        //Debug.Log("matrics(normalized): " + print);

        // update timeFactor
        timeFactor = LerpTimeFactor(timeKnob.knobValue);

        // computer time factored metrics;
        for (int i = 0; i < metrics.Length; i++)
        {
            metrics[i] = metrics[i]  * timeFactor;
        }

        // update radar chart
        radarChart.metrics = metrics;
    }

    void UpdateBarChart(JsonData jsonData)
    {

        // update timeFactor
        timeFactor = LerpTimeFactor(timeKnob.knobValue);
        
        metrics = new float[timeFactors.Length];

        // if density
        float dimensionMetric = radarChart.dimensionMetrics[0] / timeFactor;
        for(int i = 0; i < metrics.Length; i++)
        {
            metrics[i] = dimensionMetric * timeFactors[i] * metricsFactor * barMetricsFactor;
        }

        // update bar chart
        barChart.metrics = metrics;
    }

    void UpdateHeatmap(JsonData jsonData)
    {
        List<Vector4> tmpVector4List = new List<Vector4>();
        foreach (UnitInfoData unitInfo in jsonData.fixed_units)
        {
            // heatmap's posx posy radius and intensity
            Vector4 tmpVector4 = new Vector4(unitInfo.x, unitInfo.y, heatmapRangeFactorFixedUnit, heatmapIntensityFactorFixedUnit * timeFactor);
            tmpVector4List.Add(tmpVector4);
        }
        // RZ 190515 temp fix for filming
        /*
        foreach (UnitInfoData unitInfo in jsonData.free_units)
        {
            // heatmap's posx posy radius and intensity
            Vector4 tmpVector4 = new Vector4(unitInfo.x, unitInfo.y, heatmapRangeFactorFreeUnit, heatmapIntensityFactorFreeUnit * timeFactor);
            tmpVector4List.Add(tmpVector4);
        }
        */
        // foreach (GameObject freeUnitGO in app.freeUnits)
        // {
        //     // heatmap's posx posy radius and intensity
        //     Vector4 tmpVector4 = new Vector4(freeUnitGO.transform.localPosition.x, freeUnitGO.transform.localPosition.z, heatmapRangeFactorFreeUnit, heatmapIntensityFactorFreeUnit * timeFactor);
        //     tmpVector4List.Add(tmpVector4);
        // }
        for (int i = 0; i < heatmap.count; i++)
        {
            if (i < tmpVector4List.Count)
            {
                heatmap.positions[i].x = tmpVector4List[i].x;
                heatmap.positions[i].z = tmpVector4List[i].y;
                heatmap.radiuses[i] = tmpVector4List[i].z;
                heatmap.intensities[i] = tmpVector4List[i].w;
            }
            else
            {
                heatmap.positions[i].x = 0f;
                heatmap.positions[i].z = 0f;
                heatmap.radiuses[i] = 0f;
                heatmap.intensities[i] = 0f;
            }
        }
    }

    float FreeUnitRot2Capacity(float rot)
    {
        float capacity = (1f - rot / 360f) * (freeUnitCapacityMax - freeUnitCapacityMin) + freeUnitCapacityMin;
        return capacity;
    }

    float LerpTimeFactor(float timeKnobValue)
    {
        int numFactors = timeFactors.Length;
        int numSessions = numFactors - 1;
        float sessionSize = 1f / numSessions;
        int currentSession = (int)Mathf.Floor(timeKnobValue / sessionSize);
        float sessionStart = sessionSize * currentSession;
        float sessionEnd = sessionSize * (currentSession + 1);
        float valueStart = timeFactors[currentSession];
        float valueEnd = timeFactors[currentSession + 1];
        float timeFactor = valueStart + (valueEnd - valueStart) / sessionSize * (timeKnobValue - sessionStart);
        return timeFactor;
    }
}
