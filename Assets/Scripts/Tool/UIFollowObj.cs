using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class UIFollowObj : MonoBehaviour {

    public RectTransform targetUI;
    public GameObject go;

    public Camera uicamera;
    public Camera objcamera;

    float heightRatio;
    float widthRatio;

    public Vector2 screenRect;
    public bool startUpdate = false;
    // Use this for initialization
    void Start () {
        go = gameObject;
        objcamera = Camera.main;
        uicamera = GameObject.FindGameObjectWithTag("UICamera").GetComponent<Camera>();
    }
	
    public void SetParam(RectTransform parentui)
    {
        objcamera = Camera.main;
        targetUI = parentui;
        uicamera = GameObject.FindGameObjectWithTag("UICamera").GetComponent<Camera>();
        go = gameObject;
        startUpdate = true;
    }

	// Update is called once per frame
	void Update () {
        if (startUpdate)
        {
            //SetPosByRatio(go, GetRatioByPos(parentUI.transform.position));
            UpdatePosition();
        }
    }

    public void UpdatePosition()
    {
        Vector3 viewPos = this.objcamera.WorldToViewportPoint(transform.position);

        if(viewPos.x<0||viewPos.x>1||viewPos.y<0||viewPos.y>1)
        {
            targetUI.gameObject.SetActive(false);
            return;
        }else
        {
            targetUI.gameObject.SetActive(true);
        }
        RectTransform rect = targetUI.transform.parent.GetComponent<RectTransform>();
        Vector3 screenPos = this.objcamera.WorldToScreenPoint(transform.position);
        Vector2 localPoint;
        if (RectTransformUtility.ScreenPointToLocalPointInRectangle(rect, screenPos, uicamera, out localPoint))
        {
            targetUI.gameObject.SetActive(true);
            targetUI.anchoredPosition = localPoint;
        }
        else
        {
            targetUI.gameObject.SetActive(false);
        }
    }
}
