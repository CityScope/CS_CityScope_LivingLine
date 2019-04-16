using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class TimeKnob : MonoBehaviour
{
    [Range(0, 1f)]
    public float knobValue = 0.5f; // 0.0f to 1.0f
    public GameObject pointerActiveSource;
    public GameObject pointerActiveRoot;
    public GameObject pointerInactiveSource;
    public GameObject pointerInactiveRoot;
    public float rotDegTotal = 300.0f;
    public int pointerNumTotal = 101;
    public string[] valueTexts; // 1 week, 2 weeks, 1 month, 2 months, 6 months, 
    public TextMesh valueText;

    public int pointerNumActive;
    public int prevPointerNumActive; // better use this than knobValue because it's more stable
    public float rotDegInterval;
    public List<GameObject> pointerActiveList;
    public List<GameObject> pointerInactiveList;

    public int numValueTexts;

    void Start()
    {
        rotDegInterval = rotDegTotal / (pointerNumTotal - 1);
        pointerNumActive = (int)Mathf.Round(knobValue * pointerNumTotal);
        prevPointerNumActive = pointerNumActive;

        // copy and rotate active and inactive pointers
        for (int i = 0; i < pointerNumTotal; i++)
        {
            GameObject go1 = Instantiate(pointerActiveSource, pointerActiveRoot.transform);
            go1.transform.localEulerAngles = new Vector3(0.0f, rotDegInterval * i, 0.0f);
            go1.SetActive(false);
            pointerActiveList.Add(go1);
            GameObject go2 = Instantiate(pointerInactiveSource, pointerInactiveRoot.transform);
            go2.transform.localEulerAngles = new Vector3(0.0f, rotDegInterval * i, 0.0f);
            go2.SetActive(false);
            pointerInactiveList.Add(go2);
        }

        numValueTexts = valueTexts.Length;
    }

    void Update()
    {
        // calc active and inactive pointer number
        pointerNumActive = (int)Mathf.Round(knobValue * pointerNumTotal);

        if (pointerNumActive != prevPointerNumActive)
        {
            // enable or disable active and inactive pointers
            for (int i = 0; i < pointerNumTotal; i++)
            {
                if (i <= pointerNumActive)
                {
                    pointerActiveList[i].SetActive(true);
                    pointerInactiveList[i].SetActive(false);
                }
                else
                {
                    pointerActiveList[i].SetActive(false);
                    pointerInactiveList[i].SetActive(true);
                }
            }

            // update value text
            valueText.text = valueTexts[(int)Mathf.Round(knobValue * (numValueTexts - 1))];

            // prev value for comparison
            prevPointerNumActive = pointerNumActive;
        }
    }
}
