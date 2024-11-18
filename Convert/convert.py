import pandas as pd
import json

def read_csv_file(file_path):
    try:
        # 读取CSV文件
        df = pd.read_csv(file_path)
        return df
    except FileNotFoundError:
        print(f"错误：找不到文件 '{file_path}'")
        return None
    except Exception as e:
        print(f"读取CSV文件时发生错误:{str(e)}")
        return None

def custom_format_json(df):
    """
    自定义JSON格式转换函数
    目标格式:
    [
        {
            "instruction": "Predict the ActualTakeOffDateTime of the flight.",
            "input": "Flight_number: CZ5992, Flight_date: 2023/12/26...", 
            "output": "2023/12/26 20:50"
        },
        ...
    ]
    """
    # 创建结果列表
    result = []
    
    for index, row in df.iterrows():
        # 将每行数据转换为字典
        row_dict = row.to_dict()
        
        # 创建输入字符串
        input_str = (
            f"Flight_number: {row_dict['Flight_number']}, "
            f"Flight_date: {row_dict['Flight_date']}, "
            f"ScheduledDateTime: {row_dict['ScheduledDateTime']}, "
            f"CheckInOpenDateTime: {row_dict['CheckInOpenDateTime']}, "
            f"CheckInCloseDateTime: {row_dict['CheckInCloseDateTime']}, "
            f"Route: {row_dict['Route']}"
        )

        # 创建每条记录的字典
        record = {
            "instruction": "Predict the ActualTakeOffDateTime of the flight.",
            "input": input_str,
            "output": row_dict["ActualTakeOffDateTime"]
        }
        
        result.append(record)
    
    return result

def convert_to_json(df, output_file_path, custom_format=True):
    try:
        if custom_format:
            # 使用自定义格式转换
            json_data = custom_format_json(df)
            # 将Python列表转换为JSON字符串
            json_str = json.dumps(json_data, ensure_ascii=False, indent=4)
        else:
            # 使用原有的pandas直接转换
            json_str = df.to_json(orient='records', force_ascii=False, indent=4)
        
        # 将JSON数据写入文件
        with open(output_file_path, 'w', encoding='utf-8') as f:
            f.write(json_str)
        print(f"成功将数据转换并保存到: {output_file_path}")
        return True
    except Exception as e:
        print(f"转换JSON时发生错误: {str(e)}")
        return False

def main():
    # 使用示例
    csv_file_path = 'shafis-d.csv'  # 替换为你的CSV文件路径
    json_file_path = 'output.json'    # 输出的JSON文件路径
    
    data = read_csv_file(csv_file_path)
    
    if data is not None:
        print("成功读取CSV文件:")
        print(data.head())  # 显示前5行数据
        
        # 使用自定义格式转换为JSON并保存
        convert_to_json(data, json_file_path, custom_format=True)

if __name__ == "__main__":
    main()
