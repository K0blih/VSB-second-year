import xml.etree.ElementTree as ET

def create_student(xml_root, student_id):
    '''
    Vytvořte studenta dle loginu.
    Ujistěte se, že student neexistuje, jinak: raise Exception('student already exists')
    '''
    for student in xml_root.findall('student'):
        if student.get('student_id') == student_id:
            raise Exception('student already exists')

    xml_root.append(ET.Element('student', {'student_id': student_id}))
    


def remove_student(xml_root, student_id):
    '''
    Odstraňte studenta dle loginu
    '''
    for student in xml_root.findall('student'):
        if student.get('student_id') == student_id:
            xml_root.remove(student)


def set_task_points(xml_root, student_id, task_id, points):
    '''
    Přepište body danému studentovi u jednoho tasku
    '''
    for student in xml_root.findall('student'):
        if student.get('student_id') == student_id:
            for task in student.findall('task'):
                if task.get('task_id') == task_id:
                    task.text = str(points)
                    return


def create_task(xml_root, student_id, task_id, points):
    '''
    Pro daného studenta vytvořte task s body.
    Ujistěte se, že task (s task_id) u studenta neexistuje, jinak: raise Exception('task already exists')
    '''
    for student in xml_root.findall('student'):
        if student.get('student_id') == student_id:
            for task in student.findall('task'):
                if task.get('task_id') == task_id:
                    raise Exception('task already exists')

            new_task = ET.Element('task', {'task_id': task_id})
            new_task.text = str(points)
            student.append(new_task)
            return
            


def remove_task(xml_root, task_id):
    '''
    Napříč všemi studenty smažte task s daným task_id
    '''
    for student in xml_root.findall('student'):
        for task in student.findall('task'):
            if task.get('task_id') == task_id:
                student.remove(task)
