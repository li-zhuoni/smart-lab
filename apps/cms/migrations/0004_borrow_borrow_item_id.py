# Generated by Django 3.0.7 on 2020-11-12 09:57

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('cms', '0003_borrow'),
    ]

    operations = [
        migrations.AddField(
            model_name='borrow',
            name='borrow_item_id',
            field=models.CharField(blank=True, max_length=200, null=True),
        ),
    ]
