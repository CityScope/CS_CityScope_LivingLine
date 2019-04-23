using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using BehaviorDesigner.Runtime;

public class FixedUnit : MonoBehaviour
{
    public Fixed_UnitData data;
    public GameObject showUnitGo;
    public int lastShowType = -1;

    public BoxMorphKeyPts boxMorphKeyPtsMain;

    void Start()
    {

    }

    public bool Show(UnitInfoData infoData)
    {
        if (infoData == null)
        {
            if (showUnitGo != null)
            {
                showUnitGo.SetActive(false);
            }
            return false;
        }

        if (Mathf.Abs(infoData.x + App.c_UnitXoffset - this.transform.position.x) > 3.5 || Mathf.Abs(infoData.y - this.transform.position.z) > 3.5)
        {
            return false;
        }

        if (lastShowType == infoData.type && showUnitGo)
        {
            showUnitGo.SetActive(true);
            return true;
        }

        string sourceName = null;
        switch (infoData.type)
        {
            case 0:
                sourceName = "Future Education Lab";
                break;
            case 1:
                sourceName = "Future Food Lab";
                break;
            case 2:
                sourceName = "Future Art Lab";
                break;
            case 3:
                sourceName = "Incubator";
                break;
            case 4:
                sourceName = "Co-working Space";
                break;
            case 5:
                sourceName = "Market";
                break;
            case 6:
                sourceName = "Community Center";
                break;
        }

        if (showUnitGo)
        {
            Destroy(showUnitGo);
        }
        var go = GameObject.Instantiate<GameObject>(Resources.Load<GameObject>(sourceName));
        go.transform.parent = this.transform;
        go.transform.localPosition = Vector3.zero;
        go.transform.localEulerAngles = Vector3.zero;
        go.transform.localScale = Vector3.one;
        // RZ TODO: might have memory leak
        // follow main box morph
        go.GetComponent<ManuallyAddBoxMorphGO>().boxMorphKeyPts = boxMorphKeyPtsMain;
        showUnitGo = go;

        lastShowType = infoData.type;

        UpdateAI();
        return true;
    }

    private void UpdateAI()
    {

        List<GameObject> goList = SingletonTMono<App>.Instance.GetNearListAI(this.transform.position);
        foreach (GameObject go in goList)
        {
            BehaviorDesigner.Runtime.BehaviorTree tree = go.GetComponent<BehaviorDesigner.Runtime.BehaviorTree>();
            SharedTransform st = new SharedTransform();
            st.Value = this.transform;
            tree.SetVariable("EventTarget", st);

            SharedBool sb = new SharedBool();
            sb.Value = true;
            tree.SetVariable("EventTargetFlag", sb);
        }
        
        for (int i = 0; i < SingletonTMono<App>.Instance.addUnitCreateAINum; i++)
        {
            SingletonTMono<App>.Instance.CreateAI(this.transform.position);
            //SingletonTMono<App>.Instance.CreateAI();
        }

    }

}
