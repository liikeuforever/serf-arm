def convert_newline_to_comma(input_file, output_file):
    try:
        with open(input_file, 'r', encoding='utf-8') as infile:
            data = infile.read()
        
        # Split the data by newline characters and join with commas
        data = data.strip().split('\n')
        comma_separated_data = ','.join(data)
        
        with open(output_file, 'w', encoding='utf-8') as outfile:
            outfile.write(comma_separated_data)
        
        print(f"Data has been successfully converted and saved to {output_file}")
    except Exception as e:
        print(f"An error occurred: {e}")

# Example usage
input_file = 'Wind-Speed.csv'
output_file = 'ws.csv'
convert_newline_to_comma(input_file, output_file)
