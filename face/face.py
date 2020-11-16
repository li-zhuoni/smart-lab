# -*- coding: utf-8 -*-
import cv2
import sys
import base64
import json
from aip.face import AipFace

dicName = {'user01': 'USR01', 'user02': 'USR01', 'user03': 'USR01', 'user04': 'USR01'}  # id 和名字对应关系


def FaceRec_Baidu(cam_index):
    cap = cv2.VideoCapture(cam_index)
    detector = cv2.CascadeClassifier(
        '$HOME/opencv-build/installed_x86/share/opencv4/haarcascades/haarcascade_frontalface_alt2.xml')
    APP_ID = '22856274'
    API_KEY = 'Lpyr3Qur1lZ1D7zfBbhRQtIZ'
    SECRET_KEY = 'vTBDKxTGynVGQuZqU6aLo7B6jIrgFaMq'
    client = AipFace(APP_ID, API_KEY, SECRET_KEY)
    state = False
    while True:
        ret, img = cap.read()
        if not ret:
            break
        # print('开始')
        grey = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        # print('灰度图')
        face = detector.detectMultiScale(grey, 1.3, 5)
        # print(len(face))
        for (x, y, w, h) in face:
            cv2.rectangle(img, (x, y), (x + w, y + h), (0, 255, 0), 2)
        # print('绘图')
        face64 = img2Base64(img)
        if len(face) == 0:
            state = False
            cv2.imshow('frame', img)
            # continue

        # print('变换成face64')
        img_Type = 'BASE64'
        groupIdList = "tech"
        # print('人脸搜索')
        if state == False and len(face) != 0:
            ret = client.search(face64, img_Type, groupIdList)
            if ret['error_msg'] == 'SUCCESS':
                userid = ret['result']['user_list'][0]['user_id']
                print('识别成功！' + dicName[userid] + ': 您好')
                cv2.putText(img, userid,
                            (x + 30, y + 30),  # 坐标
                            cv2.FONT_HERSHEY_SIMPLEX,  # 字体
                            1,  # 字号
                            (255, 0, 255),  # 颜色
                            2)  # 字的线宽
                state = True
        cv2.imshow('frame', img)
        # if js['error_msg'] == 'SUCCESS':
        # print(js['result'])
        # else:
        # cv2.waitKey(100)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
    # cap.release()
    # cv2.destroyAllWindows()


# def FaceSearch(image_Base64,imageType):
# opencv 抽出来的图片转base64格式
def img2Base64(image):
    image = cv2.imencode('.jpg', image)[1]
    base_data = str(base64.b64encode(image))[2:-1]
    return base_data


if __name__ == '__main__':
    # print('参数：%s'%(sys.argv[1]))
    cam = int(sys.argv[1])
    FaceRec_Baidu(cam)