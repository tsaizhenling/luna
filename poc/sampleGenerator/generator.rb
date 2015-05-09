require 'httpclient'
require 'CSV'
require 'json'

INPUT_FILE = ARGV[0]
OUTPUT_FILE = ARGV[1]

http = HTTPClient.new
base = "https://api.zalora.sg/v1/products/"
outfile = File.new(OUTPUT_FILE, "w")
outfile << "sku,brand,name,color\n"
infile = File.new(INPUT_FILE, "r")
	infile.each_line do |line|
		url = base + line.gsub(/\s+/, "")
		response = http.get(url,{"format"=>"1"},{ 'Accept' => 'application/json' })
		productList = JSON.parse(response.body)
		array = productList["data"]["products"]
		if array == nil
			next
		end
		product = array.last
		attributes = product["attributes"]
		color = ""
		attributes.each_with_index do |element,index|
			if element == "color"
				color = attributes[index+1]
			end
		end
		outfile << product["config_sku"] << "," << product["brand"] << "," << product["name"] << "," << color << "\n"
	end
infile.close
outfile.close