from ultralytics import YOLO

model = YOLO("yolo26n.pt")  

results = model.predict(source="test.jpg", show=True)

for result in results:
    print(result.boxes)