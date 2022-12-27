import sys
import cv2
import folium
from folium.plugins import MarkerCluster

sys.path.extend(["folium", "./folium/shared_dir"])

YMD = "data"

def main():
    visualize_locations_with_marker_cluster(10, zoom=4)

# info.txtから音声、カメラ情報を抜き取る
def get_info():
    path = "{}/info.txt" .format(YMD)
    with open(path, mode="r") as txt:
        lines = txt.readlines()
    return lines

def visualize_locations_with_marker_cluster(df,  zoom=4):
    """日本を拡大した地図に、pandasデータフレームのlatitudeおよびlongitudeカラムをプロットする。
    """

    # txtファイルから情報を取得する
    lines = get_info()
        	
    # 図の大きさを指定する。
    f = folium.Figure(width=1000, height=500)

    # 初期表示の中心の座標を指定して地図を作成する。
    center_lat=34.686567
    center_lon=135.52000
    m = folium.Map([center_lat,center_lon], zoom_start=zoom).add_to(f)

    # 縮尺によって、マーカーがまとめられるように設定する。
    marker_cluster = MarkerCluster().add_to(m)
        
    # データフレームの全ての行のマーカーを作成する。
    for line in lines:
        # 観測データ
        times, longitude_latitude, label = line.split(" ")
        year, month, day, hour, minute, sec = times.split("-")
        label = label.split("\n")[0]
        # 時間指定
        tooltip_times = "{}/{}/{}, {}:{}:{}" .format(year, month, day, hour, minute, sec)
        # 緯度経度
        gps, longitude, latitude = longitude_latitude.split("-")
        # jpgファイルのパス
        jpg_path = "{}/jpg/{}.JPG" .format(YMD, times)
        img = cv2.imread(jpg_path, 1)
        img = cv2.rotate(img, cv2.ROTATE_180)
        cv2.imwrite(jpg_path, img)
        # wavファイルのパス
        wav_path = "{}/wav/{}.wav" .format(YMD, times)
        # html popup
        popup = '</iframe>' \
                f'<iframe src="{jpg_path}" scrolling="no" frameborder="0" width="320" height="240"></iframe> \
                  <iframe src="{wav_path}" scrolling="no" frameborder="0" width="320" height="100"></iframe>' \
                '</iframe>'
        # iconの色の変更
        if label == "0":
            icon=folium.Icon(color='blue')
        elif label == "1":
            icon=folium.Icon(color='red')
        # マーカー作成
        folium.Marker(location=[longitude, latitude], popup=popup, tooltip=tooltip_times, icon=icon).add_to(marker_cluster)
        
    m.save("beyond_oblivion.html")

if __name__=="__main__":
    main()
