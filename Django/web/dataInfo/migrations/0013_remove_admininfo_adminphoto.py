# Generated by Django 3.2.13 on 2022-06-12 05:33

from django.db import migrations


class Migration(migrations.Migration):

    dependencies = [
        ('dataInfo', '0012_alter_admininfo_adminphoto'),
    ]

    operations = [
        migrations.RemoveField(
            model_name='admininfo',
            name='adminphoto',
        ),
    ]
