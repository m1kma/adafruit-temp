# Mika Mäkelä - 2018
# AWS lambda to read weather data from the FMI open API and post data to the Adafruit IO Cloud 

from botocore.vendored import requests
import xml.etree.ElementTree as ET
import datetime

def lambda_handler(event, context):

    fmi_time_delta = datetime.datetime.now() - datetime.timedelta(minutes=60)
    fmi_time = fmi_time_delta.replace(microsecond=0).isoformat()
    
    fmi_url = 'http://data.fmi.fi/fmi-apikey/XXXXXX/wfs?request=getFeature&storedquery_id=fmi::observations::weather::simple&place=malmi,helsinki&timestep=30&maxlocations=1&starttime={0}&parameters=t2m,rh'.format(fmi_time)

    fmi_response = requests.get(fmi_url)

    xml_content = fmi_response.text

    parRhTime = None
    parRhValue = None
    
    parTempTime = None
    parTempValue = None

    root = ET.fromstring(xml_content)
        
    for members in root.findall('{http://www.opengis.net/wfs/2.0}member'):
        for member in members.findall('{http://xml.fmi.fi/schema/wfs/2.0}BsWfsElement'):
            
            parName = member.find('{http://xml.fmi.fi/schema/wfs/2.0}ParameterName')
            
            if parName.text == 'rh':
                parRhTime = member.find('{http://xml.fmi.fi/schema/wfs/2.0}Time').text
                parRhValue = member.find('{http://xml.fmi.fi/schema/wfs/2.0}ParameterValue').text
                
            if parName.text == 't2m':
                parTempTime = member.find('{http://xml.fmi.fi/schema/wfs/2.0}Time').text
                parTempValue = member.find('{http://xml.fmi.fi/schema/wfs/2.0}ParameterValue').text

    
    if parRhValue != 'NaN' or parTempValue != 'NaN':
        headers = {'X-AIO-Key': 'xxxx'}
        
        url_temp = 'https://io.adafruit.com/api/v2/xxxx/feeds/temperatureout/data'
        payload_temp = {'value': parTempValue}
        io_response = requests.post(url_temp, headers=headers, data=payload_temp)
        
        #print(io_response.text)
    
        url_rh = 'https://io.adafruit.com/api/v2/xxxx/feeds/humidityout/data'
        payload_rh = {'value': parRhValue}
        io_response = requests.post(url_rh, headers=headers, data=payload_rh)
    
        #print(io_response.text)
    
    return '{0} {1}'.format(parTempValue,parRhValue)
