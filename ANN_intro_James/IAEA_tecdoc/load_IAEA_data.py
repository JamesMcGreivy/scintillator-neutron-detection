# -*- coding: utf-8 -*-
"""
Created on Fri Aug 28 07:46:12 2020

@author: siefman1

IAEA data for reference neutron spectra and detector responses is given in a
PDF document. Need to extract the data and label it.

"""

# importing required modules
import PyPDF2
import numpy as np
import pandas as pd


# -----------------------------------------------------------------------------


def get_reference_spectrum(text):


    # From text of reference spectrum page, return dictionary of reference
    # spectra along with the energy bin structure

    # get headers for each column (name of reference spectrum)
    # there's five columns for each page, first column is energy bin
    # Strip white spaces, remove new line characters

    spectra_name = text.split('\n')[0].split()[2:]
    spectra_name = ' '.join(spectra_name) + ' \n'
    print(spectra_name)

    num_columns = text.count('Column')
    column_text = text.split('Column')

    if num_columns == 5:
        column2 = spectra_name + column_text[2].strip().replace('\n', '')[2:] # don't keep number and colon
        column3 = spectra_name + column_text[3].strip().replace('\n', '')[2:]
        column4 = spectra_name + column_text[4].strip().replace('\n', '')[2:]
        column5 = column_text[5].split('\n')[0] # data trails for this string, get only column label
        column5 = spectra_name + column5.strip().replace('\n', '')[2:]

    elif num_columns == 4:
        column2 = spectra_name + column_text[2].strip().replace('\n', '')[2:] # don't keep number and colon
        column3 = spectra_name + column_text[3].strip().replace('\n', '')[2:]
        column4 = column_text[4].split('\n')[0] # data trails for this string, get only column label
        column4 = spectra_name + column4.strip().replace('\n', '')[2:]

    elif num_columns == 6:
        column2 = spectra_name + column_text[2].strip().replace('\n', '')[2:] # don't keep number and colon
        column3 = spectra_name + column_text[3].strip().replace('\n', '')[2:]
        column4 = spectra_name + column_text[4].strip().replace('\n', '')[2:]
        column5 = spectra_name + column_text[5].strip().replace('\n', '')[2:]
        column6 = column_text[6].split('\n')[0] # data trails for this string, get only column label
        column6 = spectra_name + column6.strip().replace('\n', '')[2:]

    data_text = text[text.find('1.00E-03'):].split('\n')
    data_clean = []
    # remove empty rows where there were double '\n'
    for datum in data_text:
        if datum:
            data_clean.append(datum)

    E = []; spectrum2 = []; spectrum3 = []; spectrum4 = [];
    spectrum5 = []; spectrum6 = []

    for line in data_clean[:-1]:
        E.append(float(line[0:8]))
        spectrum2.append(float(line[8:16]))
        spectrum3.append(float(line[16:24]))
        spectrum4.append(float(line[24:32]))
        if num_columns == 5 or num_columns == 6:
            spectrum5.append(float(line[32:40]))
        if num_columns == 6:
            spectrum6.append(float(line[40:48]))

    # last line has only the last energy bin in it
    E.append(float(line[0:8]))

    if num_columns == 5:
        data_dict = {column2:np.array(spectrum2),
                     column3:np.array(spectrum3),
                     column4:np.array(spectrum4),
                     column5:np.array(spectrum5)}
                 # 'E_struct':np.array(E),

    elif num_columns == 4:
        data_dict = {column2:np.array(spectrum2),
                     column3:np.array(spectrum3),
                     column4:np.array(spectrum4)}

    elif num_columns == 6:
        data_dict = {column2:np.array(spectrum2),
                     column3:np.array(spectrum3),
                     column4:np.array(spectrum4),
                     column5:np.array(spectrum5),
                     column6:np.array(spectrum6)}

    return data_dict, E


# -----------------------------------------------------------------------------


def get_detector_response(text):

    # Take page with detector response data, take out data of 3He Bonner
    # spheres for each reference spectrum

    # Get overall name for type/source of spectrum. Each column can then be
    # some modification of the spectrum (moderation, location, etc)
    spectra_name = text.split('\n')[0].split()[2:]
    spectra_name = ' '.join(spectra_name) + ' \n'
    print(spectra_name)

    # get columns for the responses
    num_columns = text.count('Column')  # some pages only have 3 columns, others 4
    column_text = text.split('Column')
    if num_columns == 4:

        column1 = column_text[1].strip().replace('\n', '')[2:] # don't keep number and colon
        column2 = column_text[2].strip().replace('\n', '')[2:]
        column3 = column_text[3].strip().replace('\n', '')[2:]
        column4 = column_text[4].split('\n')[0].strip()[2:] # data trails for this string, get only column label

    elif num_columns == 3:
        column1 = column_text[1].strip().replace('\n', '')[2:] # don't keep number and colon
        column2 = column_text[2].strip().replace('\n', '')[2:]
        column3 = column_text[3].split('\n')[0].strip()[2:] # data trails for this string, get only column label

    elif num_columns == 5:
        column1 = column_text[1].strip().replace('\n', '')[2:] # don't keep number and colon
        column2 = column_text[2].strip().replace('\n', '')[2:]
        column3 = column_text[3].strip().replace('\n', '')[2:]
        column4 = column_text[4].strip().replace('\n', '')[2:]
        column5 = column_text[5].split('\n')[0].strip()[2:] # data trails for this string, get only column label


    # For now, only extract hte 3He data
    # number right of 3HE gives radius of bonner sphere in inches
    data_text = text[text.find('BS 3HE BARE'): text.find('BS LiI')].replace('\n', '')
    data_text = data_text.split('BS')
    data_clean = []
    # remove empty rows where there were double '\n'
    for datum in data_text:
        if datum:
            data_clean.append(datum)

    detector = []; response1 = []; response2 = [];
    response3 = []; response4 = []; response5 = []
    for line in data_clean:

        if num_columns == 4:
            detector.append(line[:-32].strip())
            response1.append(float(line[-32:-24]))
            response2.append(float(line[-24:-16]))
            response3.append(float(line[-16:-8]))
            response4.append(float(line[-8:]))

        elif num_columns == 3:
            detector.append(line[:-24].strip() )
            response1.append(float(line[-24:-16]))
            response2.append(float(line[-16:-8]))
            response3.append(float(line[-8:]))

        elif num_columns == 5:
            if line[-40].isnumeric():
                detector.append(line[:-40].strip())
                response1.append(float(line[-40:-32]))
            else:
                detector.append(line[:-39].strip())
                response1.append(float(line[-39:-32]))
            response2.append(float(line[-32:-24]))
            response3.append(float(line[-24:-16]))
            response4.append(float(line[-16:-8]))
            response5.append(float(line[-8:]))

    # make into pandas dataframe and return
    if num_columns == 4:
        detector_responses = {'detector':detector,
                             spectra_name+column1:np.array(response1),
                             spectra_name+column2:np.array(response2),
                             spectra_name+column3:np.array(response3),
                             spectra_name+column4:np.array(response4)}

    elif num_columns == 3:
        detector_responses = {'detector':detector,
                             spectra_name+column1:np.array(response1),
                             spectra_name+column2:np.array(response2),
                             spectra_name+column3:np.array(response3)}

    elif num_columns == 5:
        detector_responses = {'detector':detector,
                             spectra_name+column1:np.array(response1),
                             spectra_name+column2:np.array(response2),
                             spectra_name+column3:np.array(response3),
                             spectra_name+column4:np.array(response4),
                             spectra_name+column5:np.array(response5)}

    return detector_responses


# -----------------------------------------------------------------------------


def read_chapter(unfolding_data, start_page, end_page):

    page = start_page
    while page < end_page+1:
        print('Reading page %i' % page)
        # Read page with neutron spectra and return spectra with names
        spectrum_text = pdfReader.getPage(page).extractText() # get text from PDF page
        
        # Clean data of weird ASCII transfer issues
        spectrum_text = spectrum_text.replace('PuŒBe', 'Pu-Be')
        spectrum_text = spectrum_text.replace('AmŒBe', 'Am-Be')
        spectrum_text = spectrum_text.replace('AmŒB', 'Am-B')
        spectrum_text = spectrum_text.replace(',RE\n', ', REZ)\n')
        spectrum_text = spectrum_text.replace('SIMULATIONSPECTRA(CADARACHEANDGSF)',
                                              'SIMULATION SPECTRA (CADARACHE AND GSF)')
        spectrum_text = spectrum_text.replace('REACTOR FUEL TREATMENT,Pu REPROCESSING PLANT\n(UK)',
                                              'REACTOR FUEL TREATMENT,Pu REPROCESSING PLANT (UK)\n')
        spectrum_text = spectrum_text.replace('FUELTREATMENT,Pu REPROCESSING PLANT(USA)',
                                              'REACTOR FUEL TREATMENT,Pu REPROCESSING PLANT (USA)\n')



        [spectra, E_struct] = get_reference_spectrum(spectrum_text)

        # Read page with detector responses
        detector_text = pdfReader.getPage(page+2).extractText() # get text from PDF page

        # Clean data of weird ASCII transfer issues
        detector_text = detector_text.replace('PuŒBe', 'Pu-Be')
        detector_text = detector_text.replace('AmŒBe', 'Am-Be')
        detector_text = detector_text.replace('AmŒB', 'Am-B')
        detector_text = detector_text.replace(',RE\n', ', REZ)\n')
        detector_text = detector_text.replace('REACTOR FUEL TREATMENT,Pu REPROCESSING PLANT\n(UK)',
                                              'REACTOR FUEL TREATMENT,Pu REPROCESSING PLANT (UK)\n')
        detector_text = detector_text.replace('REACTOR FUEL TREATMENT,Pu REPROCESSING PLANT\n(USA)',
                                              'REACTOR FUEL TREATMENT,Pu REPROCESSING PLANT (USA)\n')
                
        det_response = get_detector_response(detector_text)
        
        for key, value in spectra.items():
            
            # Catch typos and formatting differences
            if 'Filter gallery,position S3' in key: # catch typo on page 183
                key = key.replace('Filter gallery,position S3', 
                                  'Filter gallery,position. S3')
                
            if 'Am-Be (PTB)' in key:
                key = key.replace('Am-Be (PTB)', 'Am-Be SPECTRA (PTB)')
#            print(key)
#            print(det_response)
            data = {'Spectrum Name':[key],
                    'Spectrum':[value],
                    'Detector Response':[det_response[key]]}

            data = pd.DataFrame.from_dict(data, orient='columns')
            unfolding_data = unfolding_data.append(data, ignore_index=True)
        page += 3

    return unfolding_data, E_struct



# -----------------------------------------------------------------------------

# creating a pdf file object
pdfFileObj = open('IAEA_TRS403.pdf', 'rb')

# creating a pdf reader object
pdfReader = PyPDF2.PdfFileReader(pdfFileObj)

# Get the neutron spectra in the file, starts at page 82
# Even pages are the spectra, odd pages are the detector responses
unfolding_data = {'Spectrum Name':[], 'Spectrum':[], 'Detector Response':[]}
unfolding_data = pd.DataFrame(unfolding_data)

# Read Chapter 4, calibration and reference spectra
unfolding_data,_ = read_chapter(unfolding_data, 81, 143)

# Read Chapter 5, operational spectra
unfolding_data, E_struct = read_chapter(unfolding_data, 152, 262)

pdfFileObj.close()

unfolding_data.to_pickle("./unfolding_data.pkl")

import matplotlib.pyplot as plt
for row in unfolding_data.iterrows():

    values1 = np.copy(row[1]['Spectrum'])
    values1 = np.insert(values1, 0, 0)  # insert 0 for step plot

    plt.step(E_struct, values1, 'k', alpha=0.25)

plt.semilogx()
plt.ylabel('Neutron Fluence')
plt.xlabel('Energy (MeV)')
plt.tight_layout()
plt.savefig('overlay_neutron_spectra.png', dpi=500)
plt.close()

radii = [0, 2.5, 3, 3.5, 4, 4.5, 5, 6, 7, 8, 9.5, 10, 12, 15, 18]
i = 0
for row in unfolding_data.iterrows():
    print(row)
    i += 1
    values1 = np.copy(row[1]['Detector Response'])
#    plt.xticks(np.arange(15), radii)
    plt.scatter(np.arange(15), values1, c='blue', alpha=0.25, s=100)

plt.ylabel('Neutron Fluence Response (-)')
plt.xlabel('Bonner Sphere Radius (in)')
plt.tight_layout()
plt.savefig('det_responses.png', dpi=500)
plt.show()
plt.close()

